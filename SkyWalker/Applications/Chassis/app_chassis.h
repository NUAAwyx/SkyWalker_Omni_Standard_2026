#ifndef APP_CHASSIS_H
#define APP_CHASSIS_H

#include "dvc_motor_dji.h"
#include "dvc_bmi088.h"
#include "dvc_dr16.h"
#include "dvc_referee.h"

#include <vector>

#define Chassis_Radis 0.15  // 底盘半径，单位：m
#define Wheel_Radis 0.01    // 轮半径，单位：m

class APP_Chassis
{
public:

    APP_Chassis();

    void Initialize_Chassis();

    void Chassis_Control();

    void Inverse_Kinematics();
    void Forward_Kinematics();
    void Velocity_Control();
    void Omega_Control();
    void Inverse_Dynamics();

    void Set_Chassis_Target_Velocity();
    void Set_Chassis_Target_Omega();
    void Set_Chassis_Now_Omega();
    void Set_Gimbal_to_Chassis_Theta(float theta);
    void Set_Chassis_Target_Force_X(float force_x);
    void Set_Chassis_Target_Force_Y(float force_y);
    void Set_Chassis_Target_Torque(float torque);

    void Power_Limit_Control();

private:

    float Wheel_Power_Limit_Control(float wheel_available_power, float wheel_consume_power);

    // 底盘轮子四个电机
    static std::vector<std::shared_ptr<DVC_Motor_DJI>> chassis_motors;
    // 底盘陀螺仪
    static std::shared_ptr<DVC_BMI088> BMI088;

    // 平动速度PID对象
    static std::shared_ptr<Alg_PID> Velocity_PID;
    // 角速度环PID对象
    static std::shared_ptr<Alg_PID> Omega_PID;

    // 底盘目标平动速度
    float Chassis_Target_Velocity_X;
    float Chassis_Target_Velocity_Y;
    // 底盘当前平动速度
    float Chassis_Now_Velocity_X;
    float Chassis_Now_Velocity_Y;
    // 底盘目标旋转角速度
    float Chassis_Target_Omega;
    // 底盘当前角速度
    float Chassis_Now_Omega;
    // 底盘目标牵引力
    float Chassis_Target_Force_X;
    float Chassis_Target_Force_Y;
    // 底盘目标转矩
    float Chassis_Target_Torque;
    // 底盘和云台的偏角
    float Gimbal_to_Chassis_Theta;

    // 当前底盘功率最大值
    float Power_Limit_Max;
    //功率衰减因数
    float Power_Factor;

};

extern std::shared_ptr<APP_Chassis>Chassis;

#endif