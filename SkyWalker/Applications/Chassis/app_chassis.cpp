#include "app_chassis.h"

std::shared_ptr<APP_Chassis>Chassis = std::make_shared<APP_Chassis>();

// 定义静态成员变量
std::vector<std::shared_ptr<DVC_Motor_DJI>> APP_Chassis::chassis_motors;
std::shared_ptr<DVC_BMI088> APP_Chassis::BMI088;
std::shared_ptr<Alg_PID> APP_Chassis::Velocity_PID;
std::shared_ptr<Alg_PID> APP_Chassis::Omega_PID;


/**
 * @brief 构造函数
 *
 */
APP_Chassis::APP_Chassis()
{
}

/**
 * @brief 底盘初始化配置
 *
 */
void APP_Chassis::Initialize_Chassis()
{
    // 创建电机
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 1, GM6020, DJI_Velocity));
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 2, M3508, DJI_Velocity));
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 3, M3508, DJI_Velocity));
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 4, M3508, DJI_Velocity));

    // 创建陀螺仪
    BMI088 = std::make_shared<DVC_BMI088>(spi2);

    // 创建平动和角速度环PID对象
    Velocity_PID = std::make_shared<Alg_PID>();
    Omega_PID = std::make_shared<Alg_PID>();

    // 设置底盘各电机PID参数
    chassis_motors[0]->DJI_PID_Velocity->Set_PID_Parameters(0,0,0,0,0,0,0);
    chassis_motors[1]->DJI_PID_Velocity->Set_PID_Parameters(0,0,0,0,0,0,0);
    chassis_motors[2]->DJI_PID_Velocity->Set_PID_Parameters(0,0,0,0,0,0,0);
    chassis_motors[3]->DJI_PID_Velocity->Set_PID_Parameters(0,0,0,0,0,0,0);

    // 设置底盘力控算法所需PID参数
    Velocity_PID->Set_PID_Parameters(0,0,0,0,0,0,0);
    Omega_PID->Set_PID_Parameters(0,0,0,0,0,0,0);
}

/**
 * @brief 底盘控制
 *
 */
void APP_Chassis::Chassis_Control()
{
    BMI088->Read_Data();

    Inverse_Kinematics();
    Forward_Kinematics();
    Velocity_Control();
    Omega_Control();
    Inverse_Dynamics();

    Power_Limit_Control();

    chassis_motors[0]->Motor_Control();

    chassis_motors[1]->Motor_Control();

    chassis_motors[2]->Motor_Control();

    chassis_motors[3]->Motor_Control();
}

/**
 * @brief 底盘运动学正解算，由轮向电机的当前角速度推出底盘当前平动速度以及旋转角速度
 *
 */
void APP_Chassis::Forward_Kinematics()
{
    float omega_0 = chassis_motors[0]->Get_Now_Omega();
    float omega_1 = chassis_motors[1]->Get_Now_Omega();
    float omega_2 = chassis_motors[2]->Get_Now_Omega();
    float omega_3 = chassis_motors[3]->Get_Now_Omega();

    Chassis_Now_Velocity_X = (-0.707 * omega_0 - 0.707 * omega_1 + 0.707 * omega_2 + 0.707 * omega_3) * Wheel_Radius / 4;
    Chassis_Now_Velocity_Y = (0.707 * omega_0 - 0.707 * omega_1 - 0.707 * omega_2 + 0.707 * omega_3) * Wheel_Radius / 4;
}

/**
 * @brief 底盘运动学逆解算，由底盘目标平动速度和目标旋转角速度推轮向电机目标角速度
 *
 */
void APP_Chassis::Inverse_Kinematics()
{
    // 左前，0号，(-√2/2 * Vx + √2/2 * Vy + W * R) / r
    chassis_motors[0]->Set_Target_Omega((-0.707 * Chassis_Target_Velocity_X + 0.707 * Chassis_Target_Velocity_Y + Chassis_Target_Omega * Chassis_Radius) / Wheel_Radius);
    // 左后，1号，(-√2/2 * Vx - √2/2 * Vy + W * R) / r
    chassis_motors[1]->Set_Target_Omega((-0.707 * Chassis_Target_Velocity_X - 0.707 * Chassis_Target_Velocity_Y + Chassis_Target_Omega * Chassis_Radius) / Wheel_Radius);
    // 右后，2号，(√2/2 * Vx - √2/2 * Vy + W * R) / r
    chassis_motors[2]->Set_Target_Omega((0.707 * Chassis_Target_Velocity_X - 0.707 * Chassis_Target_Velocity_Y + Chassis_Target_Omega * Chassis_Radius) / Wheel_Radius);
    // 右前，3号，(√2/2 * Vx + √2/2 * Vy + W * R) / r
    chassis_motors[3]->Set_Target_Omega((0.707 * Chassis_Target_Velocity_X + 0.707 * Chassis_Target_Velocity_Y + Chassis_Target_Omega * Chassis_Radius) / Wheel_Radius);
}

/**
 * @brief 平动速度PID，由平动速度推出底盘目标牵引力
 *
 */
void APP_Chassis::Velocity_Control()
{
    // 解算X方向牵引力
    Velocity_PID->Set_Target(Chassis_Target_Velocity_X);
    Velocity_PID->Set_Now(Chassis_Now_Velocity_X);
    Velocity_PID->Update();
    float force_x = Velocity_PID->Get_Output();
    Set_Chassis_Target_Force_X(force_x);

    // 解算Y方向牵引力
    Velocity_PID->Set_Target(Chassis_Target_Velocity_Y);
    Velocity_PID->Set_Now(Chassis_Now_Velocity_Y);
    Velocity_PID->Update();
    float force_y = Velocity_PID->Get_Output();
    Set_Chassis_Target_Force_Y(force_y);
}

/**
 * @brief 角速度环PID，由旋转角速度推出底盘目标转矩
 *
 */
void APP_Chassis::Omega_Control()
{
    Omega_PID->Set_Target(Chassis_Target_Omega);
    Omega_PID->Set_Now(Chassis_Now_Omega);
    Omega_PID->Update();
    const float torque = Omega_PID->Get_Output();
    Set_Chassis_Target_Torque(torque);
}

/**
 * @brief 动力学逆解算，由底盘目标牵引力和转矩推出电机扭矩，形成前馈
 *
 */
void APP_Chassis::Inverse_Dynamics()
{
    // (-√2 * Fx + √2 * Fy + T / r) / 4 * s
    chassis_motors[0]->DJI_PID_Velocity->Set_PID_FeedForward((-1.414 * Chassis_Target_Force_X + 1.414 * Chassis_Target_Force_Y + Chassis_Target_Torque / Chassis_Radius) / 4 * Wheel_Radius);
    // (-√2 * Fx - √2 * Fy + T / r) / 4 * s
    chassis_motors[1]->DJI_PID_Velocity->Set_PID_FeedForward((-1.414 * Chassis_Target_Force_X - 1.414 * Chassis_Target_Force_Y + Chassis_Target_Torque / Chassis_Radius) / 4 * Wheel_Radius);
    // (√2 * Fx - √2 * Fy + T / r) / 4 * s
    chassis_motors[2]->DJI_PID_Velocity->Set_PID_FeedForward((1.414 * Chassis_Target_Force_X - 1.414 * Chassis_Target_Force_Y + Chassis_Target_Torque / Chassis_Radius) / 4 * Wheel_Radius);
    // (√2 * Fx + √2 * Fy + T / r) / 4 * s
    chassis_motors[3]->DJI_PID_Velocity->Set_PID_FeedForward((1.414 * Chassis_Target_Force_X + 1.414 * Chassis_Target_Force_Y + Chassis_Target_Torque / Chassis_Radius) / 4 * Wheel_Radius);
}

/**
 * @brief 设置底盘目标平动速度，由遥控器给出
 *
 */
void APP_Chassis::Set_Chassis_Target_Velocity()
{
    Chassis_Target_Velocity_X = DR16->Get_Left_X() + (DR16->Get_Key_W() - DR16->Get_Key_S()) * 660.0;
    Chassis_Target_Velocity_Y = DR16->Get_Left_Y() + (-DR16->Get_Key_A() + DR16->Get_Key_D()) * 660.0;
}

/**
 * @brief 设置底盘小陀螺目标旋转角速度，由遥控器给出
 *
 */
void APP_Chassis::Set_Chassis_Target_Omega()
{
    Chassis_Target_Omega = (DR16->Get_Front_Wheel() / 660.0 + DR16->Get_Key_Shift()) * Chassis_Target_Omega_Factor;
}

/**
 * @brief 设置底盘当前旋转角速度，由陀螺仪给出
 *
 */
void APP_Chassis::Set_Chassis_Now_Omega()
{
    Chassis_Now_Omega = BMI088->Get_Gyro_Z();
}

/**
 * @brief 设置底盘与云台的偏角
 *
 */
void APP_Chassis::Set_Gimbal_to_Chassis_Theta(float theta)
{
    Gimbal_to_Chassis_Theta = theta;
}

/**
 * @brief 设置底盘X方向目标牵引力
 *
 */
void APP_Chassis::Set_Chassis_Target_Force_X(float force_x)
{
    Chassis_Target_Force_X = force_x;
}

/**
 * @brief 设置底盘Y方向目标牵引
 *
 */
void APP_Chassis::Set_Chassis_Target_Force_Y(float force_y)
{
    Chassis_Target_Force_Y = force_y;
}

/**
 * @brief 设置底盘目标转矩
 *
 */
void APP_Chassis::Set_Chassis_Target_Torque(float torque)
{
    Chassis_Target_Torque = torque;
}

/**
 * @brief 设置底盘扭矩前馈值
 *
 */
void APP_Chassis::Set_Chassis_Torque_Feedback(float feedback)
{
    Chassis_Torque_Feedback = feedback;
}

/**
 * @brief 轮向电机功率控制
 *
 * @param wheel_available_power 轮向电机可用功率
 * @param wheel_consume_power 轮向电机消耗功率
 * @return 功率因数
 */
float APP_Chassis:: Wheel_Power_Limit_Control(float wheel_available_power, float wheel_consume_power)
{
    if (wheel_consume_power <= wheel_available_power)
    {
        // 无需功率控制
        return (1.0f);
    }
    else
    {
        // 功率分配因子计算
        return (wheel_available_power / wheel_consume_power);
    }
}

/**
 * @brief 功率控制，求出功率削减因数以配合电机限制电流
 *
 */
void APP_Chassis::Power_Limit_Control()
{
    float available_power = Power_Limit_Max;
    float consume_power = 0.0f;         // 消耗的功率
    float wheel_consume_power = 0.0f;   // 轮向电机消耗的功率
    float wheel_power_single[4];        // 每个电机的功率

    for (int i = 0; i < 4; i++)
    {
        // 获取轮向电机功率估计值
        wheel_power_single[i] = chassis_motors[i]->Get_Power_Estimate();
        // 正功计入消耗, 负功计入补偿
        if (wheel_power_single[i] > 0)
        {
            consume_power += wheel_power_single[i];
            wheel_consume_power += wheel_power_single[i];
        }
        else
        {
            available_power += -wheel_power_single[i];
        }
    }

    Power_Factor = Wheel_Power_Limit_Control(available_power, wheel_consume_power);

    for (int i = 0; i < 4; i++)
    {
        if (wheel_power_single[i] > 0)
        {
            chassis_motors[i]->Set_Power_Factor(Power_Factor);
        }
        else
        {
            chassis_motors[i]->Set_Power_Factor(1.0f);
        }
    }
}