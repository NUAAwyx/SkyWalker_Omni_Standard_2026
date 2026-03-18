#ifndef APP_GIMBAL_H
#define APP_GIMBAL_H

#include "dvc_motor_dm.h"
#include "dvc_motor_dji.h"
#include "dvc_referee.h"
#include "dvc_communication.h"

class APP_Gimbal
{
public:

    APP_Gimbal();

    void Gimbal_Control();

    void Yaw_Control();
    void Pitch_Control();

private:

    static std::shared_ptr<DVC_Motor_DM> yaw_motor;
    static std::shared_ptr<DVC_Motor_DJI> pitch_motor;
};

extern std::shared_ptr<APP_Gimbal> Gimbal;


#endif