#ifndef DVC_MOTOR_DJI_H
#define DVC_MOTOR_DJI_H

#include <memory>

#include "bsp_can.h"
#include "Alg_PID.h"

#define Encoder_Max_Value 8192
#define PI 3.14159
#define RPM_to_RADPS PI / 30

enum Enum_Motor_Type
{
    M3508,
    M2006,
    GM6020
};

class DVC_Motor_DJI
{
public:

    DVC_Motor_DJI(std::shared_ptr<BSP_CAN> can, uint8_t id, Enum_Motor_Type motor_type);

    //void Init(float kp, float ki, float kd);

    void Set_Target_Angle(float angle);
    void Set_Target_Omega(float omega);
    void Set_Data_to_send(uint16_t data_to_send_);

private:

    void Handle_Transmit_Data(uint8_t *Tx_Buffer);
    void Handle_Receive_Data(const Struct_FDCAN_Receive_Management& Receive_Management);

    // DJI电机的CAN通信对象
    std::shared_ptr<BSP_CAN> DJI_CAN;

    // DJI电机的PID对象
    std::shared_ptr<Alg_PID> DJI_PID;

    // 电机控制目标量
    float Target_Angle; // 目标角度
    float Target_Omega; // 目标角速度

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
    uint16_t data_to_send;

    // 电机类型
    Enum_Motor_Type Motor_Type;

};





#endif
