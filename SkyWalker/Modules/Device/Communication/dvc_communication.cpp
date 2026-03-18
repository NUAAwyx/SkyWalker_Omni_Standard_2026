#include "dvc_communication.h"

/**
 * @brief 构造函数
 *
 */
DVC_Communication::DVC_Communication(std::shared_ptr<BSP_USART> uart_)
    :uart(uart_)
{
    uart->Register_ReceiveCallback([this](const Struct_UART_Receive_Management& Receive_Management)
    {
        this->Handle_Receive_Data(Receive_Management);
    });
}

/**
 * @brief 与NUC通信数据解析函数
 *
 */
void DVC_Communication::Handle_Receive_Data(const Struct_UART_Receive_Management& Receive_Management)
{
    // 处理自瞄数据
    if (Receive_Management.rx_buffer[0] == 'A')
    {
        memcpy(&Aim_Feedback, Receive_Management.rx_buffer, sizeof(Aim_Feedback));
    }
    // 处理IMU数据
    else if (Receive_Management.rx_buffer[0] == 0x55)
    {
        memcpy(&IMU_Feedback, Receive_Management.rx_buffer, sizeof(IMU_Feedback));
    }
}

/**
 * @brief 获取陀螺仪反馈的Pitch轴角速度
 *
 */
float DVC_Communication::Get_Gyro_Y()
{
    return IMU_Feedback.gyroy;
}

/**
 * @brief 获取陀螺仪反馈的Yaw轴角速度
 *
 */
float DVC_Communication::Get_Gyro_Z()
{
    return IMU_Feedback.gyroz;
}

/**
 * @brief 获取自瞄解算的Pitch轴角度结果
 *
 */
float DVC_Communication::Get_Pitch()
{
    return Aim_Feedback.pitch;
}

/**
 * @brief 获取自瞄解算的Yaw轴角度结果
 *
 */
float DVC_Communication::Get_Yaw()
{
    return Aim_Feedback.yaw;
}