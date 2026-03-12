#include "alg_pid.h"

/**
 * @brief 初始化PID，设定参数值
 *
 */
void Alg_PID::Init(float kp, float ki, float kd)
{
    KP = kp;
    KI = ki;
    KD = kd;
    error = 0;
    last_error = 0;
    Now = 0;
}

/**
 * @brief PID计算函数，算法核心
 *
 */
void Alg_PID::Update()
{
    float p_error;
    float i_error;
    float d_error;

    error = Target - Now;

    p_error = KP * error;
    i_error += KI * error;
    d_error = KD * (error - last_error);

    Output = p_error + i_error + d_error;

    last_error = error;
}

/**
 * @brief 获取PID计算结果
 *
 */
float Alg_PID::Get_Output()
{
    return Output;
}

/**
 * @brief 设定目标值
 *
 */
void Alg_PID::Set_Target(float target)
{
    Target = target;
}

/**
 * @brief 设定KP
 *
 */
void Alg_PID::Set_KP(float kp)
{
    KP = kp;
}

/**
 * @brief 设定KI
 *
 */
void Alg_PID::Set_KI(float ki)
{
    KI = ki;
}

/**
 * @brief 设定KD
 *
 */
void Alg_PID::Set_KD(float kd)
{
    KD = kd;
}