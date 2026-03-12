#include "alg_ekf.h"

/**
 * @brief  初始化EKF算法，初始化矩阵维度信息并为矩阵分配空间
 *
 * @param x_hatSize 状态变量维度
 * @param uSize 控制变量维度
 * @param zSize 观测量维度
 */
void Alg_EKF::Init(uint8_t x_hatSize, uint8_t uSize, uint8_t zSize)
{
    arm_mat_init_f32(&x_hat_minus_matrix_instance, 6, 1, x_hat_minus);
    arm_mat_init_f32(&P_minus_matrix_instance, 6, 6, P_minus);
}