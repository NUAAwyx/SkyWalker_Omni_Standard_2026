#ifndef DVC_MOTOR_DM_H
#define DVC_MOTOR_DM_H

#include <memory>

#include "bsp_can.h"

/**
 * @brief 达妙电机控制模式
 *
 */
enum Enum_Motor_DM_MODE
{
    Motor_DM_MODE_MIT = 0,
    Motor_DM_MODE_POSITION_VELOCITY = 1,
    Motor_DM_MODE_VELOCITY = 2,
};

/**
 * @brief 达妙电机错误类型
 *
 */
enum Enum_Motor_DM_Error_Status
{
    Motor_DM_Error_Status_DISABLE = 0x0,
    Motor_DM_Error_Status_ENABLE,
    Motor_DM_Error_Status_OVERVOLTAGE = 0x8,
    Motor_DM_Error_Status_UNDERVOLTAGE,
    Motor_DM_Error_Status_OVERCURRENT,
    Motor_DM_Error_Status_MOS_OVERTEMPERATURE,
    Motor_DM_Error_Status_ROTOR_OVERTEMPERATURE,
    Motor_DM_Error_Status_LOSE_CONNECTION,
    Motor_DM_Error_Status_MOS_OVERLOAD,
};

/**
 * @brief 达妙电机不同模式的ID偏移
 *
 */
enum Enum_Motor_DM_ID_Offset
{
    Motor_DM_MIT_ID_Offset = 0x000,
    Motor_DM_POSITION_VELOCITY_ID_Offset = 0x100,
    Motor_DM_VELOCITY_ID_Offset = 0x200,
};


class DVC_Motor_DM
{
public:

    DVC_Motor_DM(std::shared_ptr<BSP_CAN> can, Enum_Motor_DM_MODE motor_dm_mode, uint16_t receive_id, uint16_t can_id);

    void Set_Position_Designated(float position);
    void Set_Velocity_Designated(float velocity);
    void Set_Torque_Designated(float torque);
    void Set_MIT_K_P(int kp);
    void Set_MIT_K_D(int kd);

    void Set_Motor_Mode(Enum_Motor_DM_MODE mode);

private:

    void Handle_Transmit_Data(uint8_t *Tx_Buffer);
    void Handle_Receive_Data(const Struct_FDCAN_Receive_Management& Receive_Management);

    void Get_ID_Offset();
    void Set_CAN_ID(uint16_t can_id);

    void Set_MIT_CAN_Message(uint8_t* Tx_Buffer);
    void Set_Position_Velocity_CAN_Message(uint8_t* Tx_Buffer);
    void Set_Velocity_CAN_Message(uint8_t* Tx_Buffer);


    // 用于达妙电机通信的CAN接口
    std::shared_ptr<BSP_CAN> DM_CAN;

    // 达妙电机工作模式
    Enum_Motor_DM_MODE Motor_DM_Mode;

    // 控制目标量
    float p_des;   // 目标位置，单位rad
    float v_des;   // 目标速度，单位rad/s
    float t_ff;    // 目标力矩
    int MIT_K_P;   // MIT模式下的位置比例系数
    int MIT_K_D;   // MIT模式下的位置微分系数

    // CAN反馈的原始状态数据
    Enum_Motor_DM_Error_Status ERR;  // 电机反馈故障类型
    uint16_t POS;                    // 电机反馈编码器值
    uint16_t VEL;                    // 电机反馈速度
    uint16_t T;                      // 电机反馈力矩
    uint8_t T_MOS;                   // 电机反馈MOS管温度
    uint8_t T_Rotor;                 // 电机反馈转子温度

    // 达妙电机当前状态数据
    Enum_Motor_DM_Error_Status Error_Status;  // 电机当前故障类型
    uint16_t Position;                        // 电机当前编码器值
    uint16_t Omega;                           // 电机当前速度
    uint16_t Torqe;                           // 电机当前力矩
    uint8_t Temperature_MOS;                  // 电机当前MOS管温度
    uint8_t Temperature_Rotor;                // 电机当前转子温度

    // 电机CAN_ID，由上位机设定
    uint16_t CAN_ID;

    // 电机实际发送报文的ID
    uint16_t Transmit_ID;
    // 电机反馈报文ID
    uint16_t Receive_ID;

    // 电机不同模式的ID偏移量
    uint16_t ID_Offset;
};




#endif
