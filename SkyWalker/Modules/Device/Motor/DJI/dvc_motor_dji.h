#ifndef DVC_MOTOR_DJI_H
#define DVC_MOTOR_DJI_H

#include <memory>

#include "bsp_can.h"
#include "Alg_PID.h"

#define Encoder_Max_Value 8192
#define PI 3.14159
#define RPM_to_RADPS PI / 30

enum Enum_DJI_Motor_Work_Mode
{
    DJI_Position = 0,
    DJI_Velocity,
};

enum Enum_Motor_Type
{
    M3508,
    M2006,
    GM6020
};

class DVC_Motor_DJI
{
public:

    DVC_Motor_DJI(std::shared_ptr<BSP_CAN> can, uint8_t id, Enum_Motor_Type motor_type, Enum_DJI_Motor_Work_Mode work_mode);

    void Motor_Control();

    float Get_Target_Angle();
    float Get_Target_Omega();
    float Get_Now_Angle();
    float Get_Now_Omega();

    void Set_Target_Angle(float angle);
    void Set_Now_Angle(float angle);
    void Set_Target_Omega(float omega);
    void Set_Now_Omega(float omega);

    void Set_Data_to_send(float data_to_send_);

    void Set_Velocity_PID_KP(float kp_);
    void Set_Velocity_PID_KI(float ki_);
    void Set_Velocity_PID_KD(float kd_);
    void Set_Velocity_PID_Feedback(float feedback_);

    void Set_Position_PID_KP(float kp_);
    void Set_Position_PID_KI(float ki_);
    void Set_Position_PID_KD(float kd_);
    void Set_Position_PID_Feedback(float feedback_);

private:

    void Handle_Transmit_Data(uint8_t *Tx_Buffer);
    void Handle_Receive_Data(const Struct_FDCAN_Receive_Management& Receive_Management);

    // DJI电机的CAN通信对象
    std::shared_ptr<BSP_CAN> DJI_CAN;
    // DJI电机的位置PID对象
    std::shared_ptr<Alg_PID> DJI_PID_Position;
    // DJI电机的速度PID对象
    std::shared_ptr<Alg_PID> DJI_PID_Velocity;

    // 电机最大允许电流值
    uint8_t Motor_Max_Current;
    // 实际电流与控制数据的映射系数
    float Output_Current_to_Control_Data;

    // 电机减速比
    float Gearbox_Rate;

    // 电机控制目标量
    float Target_Angle; // 目标角度
    float Now_Angle;    // 当前角度
    float Target_Omega; // 目标角速度
    float Now_Omega;    // 当前角速度

    // 需要用到的各种ID
    uint8_t ESC_ID;               // 电机电调设置的ID
    uint16_t Transmit_ID;         // 电机发送报文ID
    uint32_t Receive_ID;          // 电机反馈报文ID
    uint8_t Transmit_ID_Offset;   // 电机发送报文ID偏移量

    // 电机反馈数据
    uint16_t encoder;     // 电机反馈编码器值
    uint16_t rpm;         // 电机反馈转速
    uint16_t torque;      // 电机反馈转矩电流
    uint16_t temperature; // 电机反馈温度

    // 电机状态数据
    float Angle;        // 电机当前角度
    float Omega;        // 电机当前角速度
    float Torque;       // 电机当前扭矩电流
    float Temperature;  // 电机当前温度

    // 要发送的数据
    float data_to_send;

    // 电机类型
    Enum_Motor_Type Motor_Type;
    // 电机工作类型
    Enum_DJI_Motor_Work_Mode Work_Mode;
};





#endif
