#ifndef ALG_EKF_H
#define ALG_EKF_H

#include "arm_math.h"
#include <cstdlib>

class Alg_EKF
{
public:

    void Init(uint8_t x_hatSize, uint8_t uSize, uint8_t zSize);

private:

    arm_matrix_instance_f32 x_hat_minus_matrix_instance; // 本次先验状态估计矩阵句柄
    float* x_hat_minus; // 本次先验状态估计矩阵数据指针

    arm_matrix_instance_f32 x_hat_matrix_instance; // 上次状态估计矩阵句柄
    float* x_hat; // 上次状态估计矩阵数据指针

    arm_matrix_instance_f32 P_minus_matrix_instance;   // 本次先验误差协方差矩阵句柄
    float* P_minus; // 本次先验误差协方差矩阵数据指针


};



#endif
