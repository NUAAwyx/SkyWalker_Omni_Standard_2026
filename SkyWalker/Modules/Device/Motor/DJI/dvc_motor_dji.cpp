#include "dvc_motor_dji.h"

/**
<<<<<<< HEAD
 * @brief 构造函数，设定电机基本CAN参数与注册发送接收回调函数
 *
 * @param can        电机绑定的CAN通道
 * @param id         电机的ID，由电调设定，此处仅赋值
 * @param motor_type 电机类型
 */
DVC_Motor_DJI::DVC_Motor_DJI(std::shared_ptr<BSP_CAN> can, uint8_t id, Enum_Motor_Type motor_type)
    :DJI_CAN(can), Motor_Type(motor_type), ESC_ID(id)
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
    }
    else if (Motor_Type == M3508 || Motor_Type == M2006)
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
    }

    // 注册接收回调函数，将Handle_Receive_Data函数绑定到回调函数中，当CAN类的回调函数被执行时，该函数自动执行
    DJI_CAN->Register_ReceiveCallback(Receive_ID, [this](const Struct_FDCAN_Receive_Management& Receive_Management)
    {
        this->Handle_Receive_Data(Receive_Management);
    });

    // 注册发送回调函数
    DJI_CAN->Register_TransmitCallback(Transmit_ID, Transmit_ID_Offset, [this](uint8_t* Tx_Buffer)
    {
        this->Handle_Transmit_Data(Tx_Buffer);
    });
}

// /**
//  * @brief 初始化大疆电机
//  *
//  * @param kp,ki,kd   电机的PID控制参数
//  */
// void DVC_Motor_DJI::Init(float kp, float ki, float kd)
// {
//     // 定义PID对象并初始化
//     DJI_PID = std::make_shared<Alg_PID>();
//     DJI_PID->Init(kp, ki, kd);
// }

/**
 * @brief 大疆电机发送数据处理
 *
 * @param Tx_Buffer 发送回调函数传入的用于存放要发送数据的数组
 */
void DVC_Motor_DJI::Handle_Transmit_Data(uint8_t *Tx_Buffer)
{
    Tx_Buffer[0] = data_to_send >> 8;
    Tx_Buffer[1] = data_to_send;
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
=======
 * @brief 初始化大疆电机
 *
 * @param hfdcan     CAN通信句柄
 * @param id         电机的ID，由电调设定，此处仅赋值
 * @param motor_type 电机类型
 * @param kp,ki,kd   电机的PID控制参数
 */
void DVC_Motor_DJI::Init(FDCAN_HandleTypeDef* hfdcan, uint8_t id, Enum_Motor_Type motor_type, float kp, float ki, float kd)
{
    // 设置电机ID与类型
    ID = id;
    Motor_Type = motor_type;

    // 确定电机CAN报文的发送ID，默认6020已开启电流环
    if (Motor_Type == GM6020)
    {
        if (ID <= 4)
        {
            CAN_ID = 0x1FE;
        }
        else if (ID >= 5 && ID <= 7)
        {
            CAN_ID = 0x2FF;
        }
    }
    else if (Motor_Type == M3508 || Motor_Type == M2006)
    {
        if (ID <= 4)
        {
            CAN_ID = 0x200;
        }
        else if (ID >= 5 && ID <= 7)
        {
            CAN_ID = 0x1FF;
        }
    }

    // 定义CAN通信对象并初始化
    DJI_CAN = std::make_shared<BSP_CAN>();
    DJI_CAN->Init(hfdcan, nullptr);

    // 定义PID对象并初始化
    DJI_PID = std::make_shared<Alg_PID>();
    DJI_PID->Init(kp, ki, kd);
}

/**
 * @brief 大疆电机数据处理
 *
 */
void DVC_Motor_DJI::Data_Process()
{
    // 处理反馈的原始数据
    encoder = CAN_Rx_Data[0] << 8 | CAN_Rx_Data[1];
    rpm = CAN_Rx_Data[2] << 8 | CAN_Rx_Data[3];
    torque = CAN_Rx_Data[4] << 8 | CAN_Rx_Data[5];
    temperature = CAN_Rx_Data[6] << 8;
>>>>>>> 8c29ab67429aaf2e57ea41aee37da25fe17064eb

    // 将反馈数据加工成实际可用数据
    Angle = (encoder / Encoder_Max_Value) * 2 * PI;
    Omega = rpm * RPM_to_RADPS;
    Torque = torque;
    Temperature = temperature;
<<<<<<< HEAD
=======

>>>>>>> 8c29ab67429aaf2e57ea41aee37da25fe17064eb
}

/**
 * @brief 设定目标角度
 *
 */
void DVC_Motor_DJI::Set_Target_Angle(float angle)
{
    Target_Angle = angle;
}

/**
 * @brief 设定目标角速度
 *
 */
void DVC_Motor_DJI::Set_Target_Omega(float omega)
{
    Target_Omega = omega;
}
<<<<<<< HEAD

/**
 * @brief 设置要发送的数据
 *
 */
void DVC_Motor_DJI::Set_Data_to_send(uint16_t data_to_send_)
{
    data_to_send = data_to_send_;
}
=======
>>>>>>> 8c29ab67429aaf2e57ea41aee37da25fe17064eb
