#include "dvc_motor_dji.h"

/**
 * @brief 构造函数，设定电机基本CAN参数与注册发送接收回调函数
 *
 * @param can        电机绑定的CAN通道
 * @param id         电机的ID，由电调设定，此处仅赋值
 * @param motor_type 电机类型
 * @param work_mode  电机工作类型
 */
DVC_Motor_DJI::DVC_Motor_DJI(std::shared_ptr<BSP_CAN> can, uint8_t id, Enum_Motor_Type motor_type, Enum_DJI_Motor_Work_Mode work_mode)
    :DJI_CAN(can), Motor_Type(motor_type), ESC_ID(id), Work_Mode(work_mode)
{
    // 确定电机CAN报文的发送ID，默认6020已开启电流环
    if (Motor_Type == GM6020)
    {
        if (ESC_ID <= 4)
        {
            Transmit_ID = 0x1FE;
            Transmit_ID_Offset = (ESC_ID - 1) * 2;
        }
        else if (ESC_ID >= 5 && ESC_ID <= 7)
        {
            Transmit_ID = 0x2FF;
            Transmit_ID_Offset = (ESC_ID - 5) * 2;
        }
        Receive_ID = 0x204 + ESC_ID;
        Motor_Max_Current = 3;
        Gearbox_Rate = 1;
    }
    else if (Motor_Type == M3508)
    {
        if (ESC_ID <= 4)
        {
            Transmit_ID = 0x200;
            Transmit_ID_Offset = (ESC_ID - 1) * 2;
        }
        else if (ESC_ID >= 5 && ESC_ID <= 7)
        {
            Transmit_ID = 0x1FF;
            Transmit_ID_Offset = (ESC_ID - 5) * 2;
        }
        Receive_ID = 0x200 + ESC_ID;
        Motor_Max_Current = 20;
        Gearbox_Rate = 19;
    }
    else if (Motor_Type == M2006)
    {
        if (ESC_ID <= 4)
        {
            Transmit_ID = 0x200;
            Transmit_ID_Offset = (ESC_ID - 1) * 2;
        }
        else if (ESC_ID >= 5 && ESC_ID <= 7)
        {
            Transmit_ID = 0x1FF;
            Transmit_ID_Offset = (ESC_ID - 5) * 2;
        }
        Receive_ID = 0x200 + ESC_ID;
        Motor_Max_Current = 10;
        Gearbox_Rate = 36;
    }

    Output_Current_to_Control_Data = 16384 / Motor_Max_Current;

    // 注册发送回调函数
    DJI_CAN->Register_TransmitCallback(Transmit_ID, Transmit_ID_Offset, [this](uint8_t* Tx_Buffer)
    {
        this->Handle_Transmit_Data(Tx_Buffer);
    });

    // 注册接收回调函数，将Handle_Receive_Data函数绑定到回调函数中，当CAN类的回调函数被执行时，该函数自动执行
    DJI_CAN->Register_ReceiveCallback(Receive_ID, [this](const Struct_FDCAN_Receive_Management& Receive_Management)
    {
        this->Handle_Receive_Data(Receive_Management);
    });

    // 创建电机PID位置环对象
    DJI_PID_Position = std::make_shared<Alg_PID>();
    // 创建电机PID速度环对象
    DJI_PID_Velocity = std::make_shared<Alg_PID>();
}

/**
 * @brief 大疆电机实际控制函数
 *
 */
void DVC_Motor_DJI::Motor_Control()
{
    float current = 0;

    if (Work_Mode == DJI_Position)
    {
        float temp_target_omega;

        DJI_PID_Position->Set_Target(Target_Angle);
        DJI_PID_Position->Set_Now(Now_Angle);
        DJI_PID_Position->Update();
        temp_target_omega = DJI_PID_Position->Get_Output();
        DJI_PID_Velocity->Set_Target(temp_target_omega);
        DJI_PID_Velocity->Set_Now(Omega);
        DJI_PID_Velocity->Update();
        current = DJI_PID_Velocity->Get_Output();
    }
    else if (Work_Mode == DJI_Velocity)
    {
        DJI_PID_Velocity->Set_Target(Target_Omega);
        DJI_PID_Velocity->Set_Now(Now_Omega);
        DJI_PID_Velocity->Update();
        current = DJI_PID_Velocity->Get_Output();
    }

    if (current >  Motor_Max_Current) current =  Motor_Max_Current;
    if (current < -Motor_Max_Current) current = -Motor_Max_Current;

    Set_Output_Current(current * Output_Current_to_Control_Data);

    Motor_DJI_Power_Limit_Control();
}

/**
 * @brief 大疆电机发送数据处理
 *
 * @param Tx_Buffer 发送回调函数传入的用于存放要发送数据的数组
 */
void DVC_Motor_DJI::Handle_Transmit_Data(uint8_t *Tx_Buffer)
{
    float data_to_send = Output_Current;

    Tx_Buffer[0] = (static_cast<int16_t>(data_to_send)) >> 8;
    Tx_Buffer[1] = static_cast<int16_t>(data_to_send);
}

/**
 * @brief 大疆电机接收数据处理
 *
 * @param Receive_Management 以常量引用方式传入的CAN接收数据管理结构体，减少调用开销防止数据被更改
 */
void DVC_Motor_DJI::Handle_Receive_Data(const Struct_FDCAN_Receive_Management& Receive_Management)
{
    // 处理反馈的原始数据
    encoder = Receive_Management.rx_data[0] << 8 | Receive_Management.rx_data[1];
    rpm = Receive_Management.rx_data[2] << 8 | Receive_Management.rx_data[3];
    torque = Receive_Management.rx_data[4] << 8 | Receive_Management.rx_data[5];
    temperature = Receive_Management.rx_data[6] << 8;

    // 将反馈数据加工成实际可用数据
    Angle = encoder * 2 * PI / Encoder_Max_Value;
    if (Angle > PI)
    {
        Angle = Angle - 2 * PI;
    }

    Omega = rpm * RPM_to_RADPS / Gearbox_Rate;
    Torque = torque;
    Temperature = temperature;

    Set_Now_Angle(Angle);
    Set_Now_Omega(Omega);
}

/**
 * @brief 设定目标角度，设定范围为-PI~PI
 *
 */
void DVC_Motor_DJI::Set_Target_Angle(float angle)
{
    angle = fmod(angle, 2*PI);
    if (angle > PI) angle -= 2*PI;
    else if (angle < -PI) angle += 2*PI;
    Target_Angle = angle;
}

/**
 * @brief 设定当前角度
 *
 */
void DVC_Motor_DJI::Set_Now_Angle(float angle)
{
    Now_Angle = angle;
}

/**
 * @brief 设定目标角速度
 *
 */
void DVC_Motor_DJI::Set_Target_Omega(float omega)
{
    Target_Omega = omega;
}

/**
 * @brief 设定当前角速度
 *
 */
void DVC_Motor_DJI::Set_Now_Omega(float omega)
{
    Now_Omega = omega;
}

/**
 * @brief 设置要发送的电流
 *
 */
void DVC_Motor_DJI::Set_Output_Current(float output_current)
{
    Output_Current = output_current;
}

/**
 * @brief 获取电机目标角度值
 *
 */
float DVC_Motor_DJI::Get_Target_Angle()
{
    return Target_Angle;
}

/**
 * @brief 获取电机目标角速度值
 *
 */
float DVC_Motor_DJI::Get_Target_Omega()
{
    return Target_Omega;
}

/**
 * @brief 获取电机当前角度值
 *
 */
float DVC_Motor_DJI::Get_Now_Angle()
{
    return Angle;
}

/**
 * @brief 获取当前电机角速度值
 *
 */
float DVC_Motor_DJI::Get_Now_Omega()
{
    return Omega;
}

/**
 * @brief 获取电机功率估计值
 *
 */
float DVC_Motor_DJI::Get_Power_Estimate()
{
    return Power_Estimate;
}

/**
 * @brief 设置功率衰减因数
 *
 */
void DVC_Motor_DJI::Set_Power_Factor(float power_factor)
{
    Power_Factor = power_factor;
}

/**
 * @brief 估计功率值
 *
 * @param K_0       电机建模系数
 * @param K_1       电机建模系数
 * @param K_2       电机建模系数
 * @param A         电机建模系数
 * @param Current   电流
 * @param Omega     角速度
 * @return
 */
float DVC_Motor_DJI::power_calculate(float K_0, float K_1, float K_2, float A, float Current, float Omega)
{
    return (K_0 * Current * Omega + K_1 * Omega * Omega + K_2 * Current * Current + A);
}

/**
 * @brief 功率控制算法, 修改电流目标值
 *
 */
void DVC_Motor_DJI::Motor_DJI_Power_Limit_Control()
{
    // 计算功率估计值
    Power_Estimate = power_calculate(Power_K_0, Power_K_1, Power_K_2, Power_A, Output_Current, Omega);

    // 若功率为正则考虑功率控制限制
    if (Power_Estimate > 0.0f)
    {
        if (Power_Factor >= 1.0f)
        {
            // 无需功率控制
        }
        else
        {
            // 需要功率控制

            // 根据功率估计公式解一元二次方程求电流值
            float a = Power_K_2;
            float b = Power_K_0 * Omega;
            float c = Power_A + Power_K_1 * Omega * Omega - Power_Factor * Power_Estimate;
            float delta, h;
            delta = b * b - 4 * a * c;
            if (delta < 0.0f)
            {
                // 无解
                Power_Limit_Current = 0.0f;
            }
            else
            {
                arm_sqrt_f32(delta, &h);
                float result_1, result_2;
                result_1 = (-b + h) / (2.0f * a);
                result_2 = (-b - h) / (2.0f * a);

                // 两个潜在的可行电流值, 取绝对值最小的那个
                if ((result_1 > 0.0f && result_2 < 0.0f) || (result_1 < 0.0f && result_2 > 0.0f))
                {
                    if ((Power_Limit_Current > 0.0f && result_1 > 0.0f) || (Power_Limit_Current < 0.0f && result_1 < 0.0f))
                    {
                        Power_Limit_Current = result_1;
                    }
                    else
                    {
                        Power_Limit_Current = result_2;
                    }
                }
                else
                {
                    if (fabs(result_1) < fabs(result_2))
                    {
                        Power_Limit_Current = result_1;
                    }
                    else
                    {
                        Power_Limit_Current = result_2;
                    }
                }
            }
            // 将功率控制后的电流赋值给输出电流做最终输出
            Output_Current = Power_Limit_Current;

        }
    }
}