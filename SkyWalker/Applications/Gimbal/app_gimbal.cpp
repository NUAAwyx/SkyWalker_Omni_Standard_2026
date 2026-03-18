#include "app_gimbal.h"

std::shared_ptr<APP_Gimbal> Gimbal = std::make_shared<APP_Gimbal>();

/**
 * @brief 云台构造函数
 *
 */
APP_Gimbal::APP_Gimbal()
{
    yaw_motor = std::make_shared<DVC_Motor_DM>(fdcan2, Motor_DM_MODE_MIT, 0x11, 0x01, 3.141593, 45, 18, DM_Velocity);
    pitch_motor = std::make_shared<DVC_Motor_DJI>(fdcan1,1,GM6020,DJI_Position);
}

/**
 * @brief 云台控制
 *
 */
void APP_Gimbal::Gimbal_Control()
{
    Yaw_Control();
    Pitch_Control();
}

/**
 * @brief Yaw电机控制
 *
 */
void APP_Gimbal::Yaw_Control()
{

}

/**
 * @brief Pitch电机控制
 *
 */
void APP_Gimbal::Pitch_Control()
{
    pitch_motor->Set_Target_Angle(3.14);
    pitch_motor->Motor_Control();
}
