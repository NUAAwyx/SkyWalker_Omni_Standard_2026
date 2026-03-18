#ifndef DVC_COMMUNICATION_H
#define DVC_COMMUNICATION_H

#include "bsp_usart.h"

/**
 * @brief 自瞄反馈数据结构体
 *
 */
struct Struct_Aim_Feedback
{
    bool mode;
    bool fire;
    float yaw;
    float yaw_vel;
    float yaw_acc;
    float pitch;
    float pitch_vel;
    float pitch_acc;
}__attribute__((packed));

/**
 * @brief IMU反馈数据结构体
 *
 */
struct Struct_IMU_Feedback
{
    float accx;   // 加速度X
    float accy;   // 加速度Y
    float accz;   // 加速度Z
    float gyrox;  // 角速度X
    float gyroy;  // 角速度Y
    float gyroz;  // 角速度Z
    float roll;   // 欧拉角roll
    float pitch;  // 欧拉角pitch
    float yaw;    // 欧拉角yaw
}__attribute__((packed));

class DVC_Communication
{
public:

    DVC_Communication(std::shared_ptr<BSP_USART> uart_);

    void Handle_Receive_Data(const Struct_UART_Receive_Management& Receive_Management);

    float Get_Gyro_Y();
    float Get_Gyro_Z();

    float Get_Pitch();
    float Get_Yaw();

private:

    // 用于与NUC通信的串口
    std::shared_ptr<BSP_USART> uart;

    // 自瞄计算结果结构体
    Struct_Aim_Feedback Aim_Feedback;

    // IMU反馈数据结构体
    Struct_IMU_Feedback IMU_Feedback;

};



#endif