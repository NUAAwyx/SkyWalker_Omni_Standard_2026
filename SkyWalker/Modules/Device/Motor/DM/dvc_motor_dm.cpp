#include "dvc_motor_dm.h"

// 清除电机错误信息, 传统模式有效
uint8_t DM_Motor_CAN_Message_Clear_Error[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfb};
// 使能电机, 传统模式有效
uint8_t DM_Motor_CAN_Message_Enter[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc};
// 失能电机, 传统模式有效
uint8_t DM_Motor_CAN_Message_Exit[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfd};
// 保存当前电机位置为零点, 传统模式有效
uint8_t DM_Motor_CAN_Message_Save_Zero[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe};

/**
 * @brief 初始化DM电机，包括定义CAN对象，确定帧ID，以及使能电机
 * （达妙可以用FDCAN，大疆只能用经典CAN，所以大疆电机部分没有can_type的配置部分）
 *
 */
void DVC_MOTOR_DM::Init(FDCAN_HandleTypeDef* hfdcan, CAN_Type can_type)
{
    // 定义大喵电机的CAN对象并初始化
    DM_CAN = std::make_shared<BSP_CAN>();
    DM_CAN->Set_CAN_Type(can_type);
    DM_CAN->Init(hfdcan ,  nullptr);

    // 确定电机CAN报文要发送的ID
    ID = CAN_ID + ID_Offset;

    // 使能电机
    DM_CAN->Send_Data(&hfdcan2, ID, DM_Motor_CAN_Message_Clear_Error, 8); // 发送清除错误信息的CAN消息
    DM_CAN->Send_Data(&hfdcan2, ID, DM_Motor_CAN_Message_Enter, 8); // 发送使能电机的CAN消息
}

/**
 * @brief 处理电机的反馈数据
 *
 */
void DVC_MOTOR_DM::Data_Process()
{
    // 解析CAN_Rx_Data中的数据，更新电机状态
    ERR = static_cast<Enum_Motor_DM_Error_Status>((CAN_Rx_Data[0]) >> 4);
    POS = ((CAN_Rx_Data[1]) << 8) | CAN_Rx_Data[2];
    VEL = ((CAN_Rx_Data[3]) << 4) | (CAN_Rx_Data[4] >> 4);
    T = ((CAN_Rx_Data[4]) << 4) | CAN_Rx_Data[5];
    T_MOS = CAN_Rx_Data[6];
    T_Rotor = CAN_Rx_Data[7];
}

/**
 * @brief 编辑MIT模式下的达妙电机的发送报文，kp的范围为[0,500]，kd的范围为[0,5]，
 * 根据MIT模式可以衍生出多种控制模式，如kp=0，kd不为0时，给定v_des即可实现匀速转动；kp=0,kd=0，
 * 给定t_ff即可实现给定扭矩输出，注意，对位置控制时，kd不可为0，否则会震荡标准CAN数据一帧只有8个字节，
 * MIT的控制命令格式将Position、Velocity、Kp、Kd、Torque五个参数按位组合在8个字节中。
 * 其中：Position占用2个字节16位、Velocity占用12位、Kp占用12位、Kd占用12位。
 */
void DVC_MOTOR_DM::Set_MIT_CAN_Message()
{
    uint16_t p = static_cast<uint16_t>(p_des);      // 16位位置
    uint16_t v = static_cast<uint16_t>(v_des);      // 12位速度（存于16位变量）
    uint16_t kp = static_cast<uint16_t>(MIT_K_P);   // 12位Kp
    uint16_t kd = static_cast<uint16_t>(MIT_K_D);   // 12位Kd
    uint16_t t = static_cast<uint16_t>(t_ff);       // 12位扭矩

    CAN_Tx_Data[0] = (p >> 8) & 0xFF;
    CAN_Tx_Data[1] = p & 0xFF;
    CAN_Tx_Data[2] = (v >> 4) & 0xFF;
    CAN_Tx_Data[3] = ((v & 0x0F) << 4) | ((kp >> 8) & 0x0F);
    CAN_Tx_Data[4] = kp & 0xFF;
    CAN_Tx_Data[5] = (kd >> 4) & 0xFF;
    CAN_Tx_Data[6] = ((kd & 0x0F) << 4) | ((t >> 8) & 0x0F);
    CAN_Tx_Data[7] = t & 0xFF;
}

/**
 * @brief 编辑位置速度模式下的达妙电机的发送报文
 * 要求输入的p_des，v_des均为浮点型，各占4个字节，低位在前，高位在后
 */
void DVC_MOTOR_DM::Set_Position_Velocity_CAN_Message()
{
    uint32_t p_bits, v_bits;
    std::memcpy(&p_bits, &p_des, sizeof(p_des));
    std::memcpy(&v_bits, &v_des, sizeof(v_des));

    CAN_Tx_Data[0] = (p_bits >> 0) & 0xFF;
    CAN_Tx_Data[1] = (p_bits >> 8) & 0xFF;
    CAN_Tx_Data[2] = (p_bits >> 16) & 0xFF;
    CAN_Tx_Data[3] = (p_bits >> 24) & 0xFF;
    CAN_Tx_Data[4] = (v_bits >> 0) & 0xFF;
    CAN_Tx_Data[5] = (v_bits >> 8) & 0xFF;
    CAN_Tx_Data[6] = (v_bits >> 16) & 0xFF;
    CAN_Tx_Data[7] = (v_bits >> 24) & 0xFF;
}

/**
 * @brief 编辑速度模式下的达妙电机的发送报文
 * 要求输入的v_des为浮点型，占4个字节，低位在前，高位在后
 */
void DVC_MOTOR_DM::Set_Velocity_CAN_Message()
{
    uint32_t v_bits;
    std::memcpy(&v_bits, &v_des, sizeof(v_des));

    CAN_Tx_Data[0] = (v_bits >> 0) & 0xFF;
    CAN_Tx_Data[1] = (v_bits >> 8) & 0xFF;
    CAN_Tx_Data[2] = (v_bits >> 16) & 0xFF;
    CAN_Tx_Data[3] = (v_bits >> 24) & 0xFF;
    CAN_Tx_Data[4] = 0;
    CAN_Tx_Data[5] = 0;
    CAN_Tx_Data[6] = 0;
    CAN_Tx_Data[7] = 0;
}

/**
 * @brief 设置达妙电机的位置控制目标
 *
 * @param position 目标位置，单位rad
 */
void DVC_MOTOR_DM::Set_Position_Designated(float position)
{
    p_des = position;
}

/**
 * @brief 设置达妙电机的速度控制目标
 *
 * @param velocity 目标速度，单位rad/s
 */
void DVC_MOTOR_DM::Set_Velocity_Designated(float velocity)
{
    v_des = velocity;
}

/**
 * @brief 设置达妙电机的力矩控制目标
 *
 * @param torque 目标力矩
 */
void DVC_MOTOR_DM::Set_Torque_Designated(float torque)
{
    t_ff = torque;
}

/**
 * @brief 设置达妙电机MIT模式下的位置比例系数，该参数仅在MIT模式下有效
 *
 */
void DVC_MOTOR_DM::Set_MIT_K_P(int kp)
{
    MIT_K_P = kp;
}

/**
 * @brief 设置达妙电机MIT模式下的位置微分系数，该参数仅在MIT模式下有效
 *
 */
void DVC_MOTOR_DM::Set_MIT_K_D(int kd)
{
    MIT_K_D = kd;
}

/**
 * @brief 获取达妙电机不同控制模式的ID偏移
 *
 */
void DVC_MOTOR_DM::Get_ID_Offset()
{
    switch (Motor_DM_Mode)
    {
    case Motor_DM_MODE_MIT:
        ID_Offset = Motor_DM_MIT_ID_Offset;
        break;

    case Motor_DM_MODE_POSITION_VELOCITY:
        ID_Offset = Motor_DM_POSITION_VELOCITY_ID_Offset;
        break;

    case Motor_DM_MODE_VELOCITY:
        ID_Offset = Motor_DM_VELOCITY_ID_Offset;

    default:
        break;
    }
}

/**
 * @brief 设置达妙电机的CAN_ID，实际由上位机设置，此处仅简单赋值
 *
 * @param can_id 上位机设定的电机CAN_ID
 */
void DVC_MOTOR_DM::Set_CAN_ID(uint16_t can_id)
{
    CAN_ID = can_id;
}

/**
 * @brief 设置达妙电机的控制模式，实际由上位机设置，此处仅简单赋值
 *
 * @param mode 上位机设定的电机工作状态
 */
void DVC_MOTOR_DM::Set_Motor_Mode(Enum_Motor_DM_MODE mode)
{
    Motor_DM_Mode = mode;
}