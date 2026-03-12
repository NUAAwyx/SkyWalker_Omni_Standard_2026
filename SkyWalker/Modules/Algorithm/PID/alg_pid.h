#ifndef ALG_PID_H
#define ALG_PID_H

class Alg_PID
{
public:

    void Init(float kp, float ki, float kd);

    void Update();

    void Set_KP(float kp);
    void Set_KI(float ki);
    void Set_KD(float kd);
    void Set_Target(float target);

    float Get_Output();

private:

    // PID所需变量
    float KP;          // 比例项系数
    float KI;          // 积分项系数
    float KD;          // 微分项系数
    float error;       // 当前误差
    float last_error;  // 前次误差
    float Now;         // 当前结果
    float Target;      // 目标值
    float Output;      // 输出值

};




#endif