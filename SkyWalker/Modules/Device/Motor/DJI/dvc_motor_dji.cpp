#include "dvc_motor_dji.h"

/**
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

    // 将反馈数据加工成实际可用数据
    Angle = (encoder / Encoder_Max_Value) * 2 * PI;
    Omega = rpm * RPM_to_RADPS;
    Torque = torque;
    Temperature = temperature;

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
