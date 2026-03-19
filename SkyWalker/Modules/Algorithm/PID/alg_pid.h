#ifndef ALG_PID_H
#define ALG_PID_H

#include <cmath>


class Alg_PID
{
public:

    Alg_PID();

    void Update();

    void Set_PID_Parameters(float kp, float ki, float kd, float i_out_max, float output_max, float dead_zone, float i_sep_threshold);

    void Set_PID_FeedForward(float feedforward);

    void Set_Now(float now);
    void Set_Target(float target);

    float Get_Output();

private:

    // PID所需变量
    float KP;               // 比例项系数
    float KI;               // 积分项系数
    float KD;               // 微分项系数
    float FeedForward;      // 前馈值
    float I_Out_Max;        // 积分限幅
    float Output_Max;       // 输出限幅
    float Dead_Zone;        // 死区
    float I_Sep_Threshold;  // 积分分离阈值
    float error;            // 当前误差
    float last_error;       // 前次误差
    float Now;              // 当前结果
    float Target;           // 目标值
    float Output;           // 输出值

    float p_out;            // P项输出
    float i_out;            // I项输出
    float d_out;            // D项输出
    float f_out;            // F项输出


};




#endif