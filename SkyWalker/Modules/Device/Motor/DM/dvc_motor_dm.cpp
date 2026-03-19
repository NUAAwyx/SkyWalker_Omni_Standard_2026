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
 * @brief 达妙电机构造函数，绑定CAN对象，确定帧ID，电机工作模式，使能电机
 * （达妙可以用FDCAN，大疆只能用经典CAN，所以大疆电机部分没有can_type的配置部分）
 *
 */
DVC_Motor_DM::DVC_Motor_DM(std::shared_ptr<BSP_CAN> can, Enum_Motor_DM_MODE motor_dm_mode, uint16_t receive_id, uint16_t can_id,
    float p_max, float v_max, float t_max, Enum_DM_Motor_Work_Mode work_mode)
    : DM_CAN(can), Motor_DM_Mode(motor_dm_mode), Receive_ID(receive_id), CAN_ID(can_id),
        P_Max(p_max), V_Max(v_max), T_Max(t_max), Work_Mode(work_mode)
{
    // 确定电机ID由模式造成的偏移量
    Get_ID_Offset();

    // 确定电机CAN报文要发送的ID
    Transmit_ID = CAN_ID + ID_Offset;

    // 使能电机
    //DM_CAN->Send_Data(Transmit_ID, DM_Motor_CAN_Message_Clear_Error, 8); // 发送清除错误信息的CAN消息
    //osDelay(1);
    DM_CAN->Send_Data(Transmit_ID, DM_Motor_CAN_Message_Enter, 8); // 发送使能电机的CAN消息
    //HAL_Delay(1);

    // 注册发送回调函数
    DM_CAN->Register_TransmitCallback(Transmit_ID,0,[this](uint8_t* Tx_Buffer)
    {
        this->Handle_Transmit_Data(Tx_Buffer);
    });

    // 注册接收回调函数
    DM_CAN->Register_ReceiveCallback(Receive_ID, [this](const Struct_FDCAN_Receive_Management& Receive_Management)
    {
        this->Handle_Receive_Data(Receive_Management);
    });
}

/**
 * @brief 达妙电机发送数据处理
 *
 * @param Tx_Buffer 发送回调函数传入的用于存放要发送数据的数组
 */
void DVC_Motor_DM::Handle_Transmit_Data(uint8_t *Tx_Buffer)
{
    switch (Motor_DM_Mode)
    {
    case Motor_DM_MODE_MIT:
        Set_MIT_CAN_Message(Tx_Buffer);
        break;

    case Motor_DM_MODE_POSITION_VELOCITY:
        Set_Position_Velocity_CAN_Message(Tx_Buffer);
        break;

    case Motor_DM_MODE_VELOCITY:
        Set_Velocity_CAN_Message(Tx_Buffer);
        break;
    }
}

/**
 * @brief 达妙电机接收数据处理
 *
 * @param Receive_Management 以常量引用方式传入的CAN接收数据管理结构体，减少调用开销防止数据被更改
 */
void DVC_Motor_DM::Handle_Receive_Data(const Struct_FDCAN_Receive_Management& Receive_Management)
{
    // 解析接收中断中的数据，更新电机状态
    ERR = static_cast<Enum_Motor_DM_Error_Status>((Receive_Management.rx_data[0]) >> 4);
    POS = ((Receive_Management.rx_data[1]) << 8) | Receive_Management.rx_data[2];
    VEL = ((Receive_Management.rx_data[3]) << 4) | (Receive_Management.rx_data[4] >> 4);
    T = ((Receive_Management.rx_data[4]) << 4) | Receive_Management.rx_data[5];
    T_MOS = Receive_Management.rx_data[6];
    T_Rotor = Receive_Management.rx_data[7];

    Angle = POS / 65535 * P_Max;
    Omega = VEL / 4095 * V_Max;
    Torqe = T / 4095 * T_Max;
    Temperature_MOS = T_MOS;
    Temperature_Rotor = T_Rotor;

    Set_Now_Angle(Angle);
    Set_Now_Omega(Omega);
}

/**
 * @brief 达妙电机MIT协议下只使用扭矩控制，自己写PID
 *
 */
void DVC_Motor_DM::MIT_Torque_Control()
{
    float current = 0;

    if (Work_Mode == DM_Position)
    {
        float temp_target_omega;

        DM_PID_Position->Set_Target(Target_Angle);
        DM_PID_Position->Set_Now(Now_Angle);
        DM_PID_Position->Update();
        temp_target_omega = DM_PID_Position->Get_Output();
        DM_PID_Velocity->Set_Target(temp_target_omega);
        DM_PID_Velocity->Set_Now(Omega);
        DM_PID_Velocity->Update();
        current = DM_PID_Velocity->Get_Output();
    }
    else if (Work_Mode == DM_Velocity)
    {
        DM_PID_Velocity->Set_Target(Target_Omega);
        DM_PID_Velocity->Set_Now(Now_Omega);
        DM_PID_Velocity->Update();
        current = DM_PID_Velocity->Get_Output();
    }
    Set_Torque_Designated(current);
}

/**
 * @brief 编辑MIT模式下的达妙电机的发送报文，kp的范围为[0,500]，kd的范围为[0,5]，
 * 根据MIT模式可以衍生出多种控制模式，如kp=0，kd不为0时，给定v_des即可实现匀速转动；kp=0,kd=0，
 * 给定t_ff即可实现给定扭矩输出，注意，对位置控制时，kd不可为0，否则会震荡标准CAN数据一帧只有8个字节，
 * MIT的控制命令格式将Position、Velocity、Kp、Kd、Torque五个参数按位组合在8个字节中。
 * 其中：Position占用2个字节16位、Velocity占用12位、Kp占用12位、Kd占用12位。
 */
void DVC_Motor_DM::Set_MIT_CAN_Message(uint8_t* Tx_Buffer)
{
    uint16_t p = static_cast<uint16_t>(p_des);      // 16位位置
    uint16_t v = static_cast<uint16_t>(v_des);      // 12位速度（存于16位变量）
    uint16_t kp = static_cast<uint16_t>(MIT_K_P);   // 12位Kp
    uint16_t kd = static_cast<uint16_t>(MIT_K_D);   // 12位Kd
    uint16_t t = static_cast<uint16_t>(t_ff);       // 12位扭矩

    Tx_Buffer[0] = (p >> 8) & 0xFF;
    Tx_Buffer[1] = p & 0xFF;
    Tx_Buffer[2] = (v >> 4) & 0xFF;
    Tx_Buffer[3] = ((v & 0x0F) << 4) | ((kp >> 8) & 0x0F);
    Tx_Buffer[4] = kp & 0xFF;
    Tx_Buffer[5] = (kd >> 4) & 0xFF;
    Tx_Buffer[6] = ((kd & 0x0F) << 4) | ((t >> 8) & 0x0F);
    Tx_Buffer[7] = t & 0xFF;
}

/**
 * @brief 编辑位置速度模式下的达妙电机的发送报文
 * 要求输入的p_des，v_des均为浮点型，各占4个字节，低位在前，高位在后
 */
void DVC_Motor_DM::Set_Position_Velocity_CAN_Message(uint8_t* Tx_Buffer)
{
    uint32_t p_bits, v_bits;
    std::memcpy(&p_bits, &p_des, sizeof(p_des));
    std::memcpy(&v_bits, &v_des, sizeof(v_des));

    Tx_Buffer[0] = (p_bits >> 0) & 0xFF;
    Tx_Buffer[1] = (p_bits >> 8) & 0xFF;
    Tx_Buffer[2] = (p_bits >> 16) & 0xFF;
    Tx_Buffer[3] = (p_bits >> 24) & 0xFF;
    Tx_Buffer[4] = (v_bits >> 0) & 0xFF;
    Tx_Buffer[5] = (v_bits >> 8) & 0xFF;
    Tx_Buffer[6] = (v_bits >> 16) & 0xFF;
    Tx_Buffer[7] = (v_bits >> 24) & 0xFF;
}

/**
 * @brief 编辑速度模式下的达妙电机的发送报文
 * 要求输入的v_des为浮点型，占4个字节，低位在前，高位在后
 */
void DVC_Motor_DM::Set_Velocity_CAN_Message(uint8_t* Tx_Buffer)
{
    uint32_t v_bits;
    std::memcpy(&v_bits, &v_des, sizeof(v_des));

    Tx_Buffer[0] = (v_bits >> 0) & 0xFF;
    Tx_Buffer[1] = (v_bits >> 8) & 0xFF;
    Tx_Buffer[2] = (v_bits >> 16) & 0xFF;
    Tx_Buffer[3] = (v_bits >> 24) & 0xFF;
    Tx_Buffer[4] = 0;
    Tx_Buffer[5] = 0;
    Tx_Buffer[6] = 0;
    Tx_Buffer[7] = 0;
}

/**
 * @brief 设置达妙电机的位置控制目标
 *
 * @param position 目标位置，单位rad
 */
void DVC_Motor_DM::Set_Position_Designated(float position)
{
    p_des = position;
}

/**
 * @brief 设置达妙电机的速度控制目标
 *
 * @param velocity 目标速度，单位rad/s
 */
void DVC_Motor_DM::Set_Velocity_Designated(float velocity)
{
    v_des = velocity;
}

/**
 * @brief 设置达妙电机的力矩控制目标
 *
 * @param torque 目标力矩
 */
void DVC_Motor_DM::Set_Torque_Designated(float torque)
{
    t_ff = torque;
}

/**
 * @brief 设置达妙电机MIT模式下的位置比例系数，该参数仅在MIT模式下有效
 *
 */
void DVC_Motor_DM::Set_MIT_K_P(int kp)
{
    MIT_K_P = kp;
}

/**
 * @brief 设置达妙电机MIT模式下的位置微分系数，该参数仅在MIT模式下有效
 *
 */
void DVC_Motor_DM::Set_MIT_K_D(int kd)
{
    MIT_K_D = kd;
}

/**
 * @brief 获取达妙电机不同控制模式的ID偏移
 *
 */
void DVC_Motor_DM::Get_ID_Offset()
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
 * @brief 设定角速度PID环的KP
 *
 */
void DVC_Motor_DM::Set_Velocity_PID_KP(float kp_)
{
    DM_PID_Velocity->Set_KP(kp_);
}

/**
 * @brief 设定角速度PID环的KI
 *
 */
void DVC_Motor_DM::Set_Velocity_PID_KI(float ki_)
{
    DM_PID_Velocity->Set_KI(ki_);
}

/**
 * @brief 设定角速度PID环的KD
 *
 */
void DVC_Motor_DM::Set_Velocity_PID_KD(float kd_)
{
    DM_PID_Velocity->Set_KD(kd_);
}

/**
 * @brief 设定角速度PID环的Feedback
 *
 */
void DVC_Motor_DM::Set_Velocity_PID_Feedback(float feedback_)
{
    DM_PID_Velocity->Set_FeedForward(feedback_);
}

/**
 * @brief 设定角度PID环的KP
 *
 */
void DVC_Motor_DM::Set_Position_PID_KP(float kp_)
{
    DM_PID_Position->Set_KP(kp_);
}

/**
 * @brief 设定角速度PID环的KI
 *
 */
void DVC_Motor_DM::Set_Position_PID_KI(float ki_)
{
    DM_PID_Position->Set_KI(ki_);
}

/**
 * @brief 设定角速度PID环的KD
 *
 */
void DVC_Motor_DM::Set_Position_PID_KD(float kd_)
{
    DM_PID_Position->Set_KD(kd_);
}

/**
 * @brief 设定角速度PID环的Feedback
 *
 */
void DVC_Motor_DM::Set_Position_PID_Feedback(float feedback_)
{
    DM_PID_Position->Set_FeedForward(feedback_);
}

/**
 * @brief 获取电机目标角度值
 *
 */
float DVC_Motor_DM::Get_Target_Angle()
{
    return Target_Angle;
}

/**
 * @brief 获取电机目标角速度值
 *
 */
float DVC_Motor_DM::Get_Target_Omega()
{
    return Target_Omega;
}

/**
 * @brief 获取电机当前角度值
 *
 */
float DVC_Motor_DM::Get_Now_Angle()
{
    return Angle;
}

/**
 * @brief 获取当前电机角速度值
 *
 */
float DVC_Motor_DM::Get_Now_Omega()
{
    return Omega;
}

/**
 * @brief 设定目标角度，设定范围为-PI~PI
 *
 */
void DVC_Motor_DM::Set_Target_Angle(float angle)
{
    Target_Angle = angle;
}

/**
 * @brief 设定当前角度
 *
 */
void DVC_Motor_DM::Set_Now_Angle(float angle)
{
    Now_Angle = angle;
}

/**
 * @brief 设定目标角速度
 *
 */
void DVC_Motor_DM::Set_Target_Omega(float omega)
{
    Target_Omega = omega;
}

/**
 * @brief 设定当前角速度
 *
 */
void DVC_Motor_DM::Set_Now_Omega(float omega)
{
    Now_Omega = omega;
}
