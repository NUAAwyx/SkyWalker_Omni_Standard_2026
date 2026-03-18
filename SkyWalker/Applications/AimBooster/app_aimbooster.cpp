#include "app_aimbooster.h"

std::shared_ptr<APP_AimBooster> AimBooster = std::make_shared<APP_AimBooster>();

/**
 * @brief 发射机构构造函数
 *
 */
APP_AimBooster::APP_AimBooster()
{
    feeder_wheel = std::make_shared<DVC_Motor_DJI>(fdcan3, 1, M2006, DJI_Velocity);
    friction_wheel_l = std::make_shared<DVC_Motor_DJI>(fdcan1,1,M3508,DJI_Velocity);
    friction_wheel_r = std::make_shared<DVC_Motor_DJI>(fdcan1,2,M3508,DJI_Velocity);

}

/**
 * @brief 发射机构控制
 *
 */
void APP_AimBooster::AimBooster_Control()
{
    Feeder_Wheel_Control();
    Friction_Wheel_Control();
}

/**
 * @brief 拨盘控制，通过控制转速实现每圈发射固定数量的弹丸，根据摩擦轮转速是否小于上一次的80%判定卡蛋，做回拨处理
 *
 */
void APP_AimBooster::Feeder_Wheel_Control()
{
    feeder_wheel->Set_Target_Omega(1000); // 此处值通过计算齿数与电机转速还有裁判系统限制综合得到

    feeder_wheel->Motor_Control();
}


/**
 * @brief 摩擦轮控制，注意正反转，定速控制即可，不可超过25m/s上限
 *
 */
void APP_AimBooster::Friction_Wheel_Control()
{
    friction_wheel_l->Set_Target_Omega(1000);
    friction_wheel_l->Motor_Control();
    friction_wheel_r->Set_Target_Omega(-1000);
    friction_wheel_r->Motor_Control();
}

/**
 * @brief 获取摩擦轮转速以判断拨盘是否需要反转
 *
 */
void APP_AimBooster::Get_Friction_Wheel_Omega()
{
    friction_Wheel_Omega = fabs(friction_wheel_l->Get_Now_Omega());
}
