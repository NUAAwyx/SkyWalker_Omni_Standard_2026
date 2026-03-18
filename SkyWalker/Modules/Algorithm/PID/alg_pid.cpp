#include "alg_pid.h"

/**
 * @brief PID类构造函数
 *
 */
Alg_PID::Alg_PID(float kp, float ki, float kd, float feedforward, float i_out_max, float output_max, float dead_zone, float i_sep_threshold)
    :KP(kp), KI(ki), KD(kd), FeedForward(feedforward),
     I_Out_Max(i_out_max), Output_Max(output_max), Dead_Zone(dead_zone), I_Sep_Threshold(i_sep_threshold)
{
    p_out = 0;
    i_out = 0;
    d_out = 0;
    f_out = 0;
    last_error = 0;
    Now = 0;
    Target = 0;
    Output = 0;
}

/**
 * @brief PID计算函数，算法核心
 *
 */
void Alg_PID::Update()
{
    error = Target - Now;

    d_out = KD * (error - last_error);

    // 设置死区
    if (fabs(error) <= Dead_Zone)
    {
        error = 0;
        d_out = 0;
    }

    p_out = KP * error;

    // 积分分离
    if (fabs(error) <= I_Sep_Threshold)
    {
        i_out += KI * error;
    }

    f_out = FeedForward;

    // 积分限幅
    if (i_out >= I_Out_Max)
    {
        i_out = I_Out_Max;
    }
    else if (i_out < -I_Out_Max)
    {
        i_out = -I_Out_Max;
    }

    Output = p_out + i_out + d_out + f_out;

    // 输出限幅
    if (Output > Output_Max)
    {
        Output = Output_Max;
    }
    else if (Output < -Output_Max)
    {
        Output = -Output_Max;
    }

    // 更新数据
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
 * @brief 设定当前值
 *
 */
void Alg_PID::Set_Now(float now)
{
    Now = now;
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

/**
 * @brief 设定前馈值
 *
 */
void Alg_PID::Set_FeedForward(float feedforward)
{
    FeedForward = feedforward;
}