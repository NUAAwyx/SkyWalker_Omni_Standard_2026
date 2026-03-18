#ifndef APP_AIMBOOSTER_H
#define APP_AIMBOOSTER_H

#include <memory>

#include "dvc_motor_dji.h"
#include "dvc_communication.h"
#include "dvc_referee.h"

class APP_AimBooster
{
public:

    APP_AimBooster();

    void AimBooster_Control();

    void Feeder_Wheel_Control();
    void Friction_Wheel_Control();

    void Get_Friction_Wheel_Omega();

private:

    std::shared_ptr<DVC_Motor_DJI> feeder_wheel;        // 拨盘对象
    std::shared_ptr<DVC_Motor_DJI> friction_wheel_l;    // 左侧摩擦轮对象
    std::shared_ptr<DVC_Motor_DJI> friction_wheel_r;    // 右侧摩擦轮对象

    uint8_t Shoot_Hz;   // 发弹频率，从裁判系统读取
    uint8_t friction_Wheel_Omega;   // 摩擦轮转速
};

extern std::shared_ptr<APP_AimBooster> AimBooster;

#endif