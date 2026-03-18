#include "dvc_vofa.h"

/**
 * @brief VOFA类的构造函数
 *
 */
DVC_VOFA::DVC_VOFA(std::shared_ptr<BSP_USART> uart_, Enum_VOFA_Mode mode_)
    : uart(uart_), mode(mode_)
{
    // 注册发送填充函数用于构建发送数组
    uart->Register_TransmitFiller([this](uint8_t* buffer, uint16_t* length)
    {
        this->Handle_Transmit_Data(buffer, length);
    });

    // 注册接收回调函数用于解析数据
    uart->Register_ReceiveCallback([this](const Struct_UART_Receive_Management& Receive_Management)
    {
        this->Handle_Receive_Data(Receive_Management);
    });
}

/**
 * @brief 设置发送数据
 *
 */
void DVC_VOFA::Set_Tx_Data(const std::vector<float>& data)
{
    tx_data = data;
}

/**
 * @brief JustFloat格式数据处理
 *
 */
void DVC_VOFA::Set_JustFloat_Data(uint8_t* buffer, uint16_t* length)
{
    *length = tx_data.size() * sizeof(float) + 4;

    memcpy(buffer, tx_data.data(), tx_data.size() * sizeof(float));

    buffer[*length - 4] = 0x00;
    buffer[*length - 3] = 0x00;
    buffer[*length - 2] = 0x80;
    buffer[*length - 1] = 0x7F;
}

/**
 * @brief 处理发送的数据
 *
 */
void DVC_VOFA::Handle_Transmit_Data(uint8_t* buffer, uint16_t* length)
{
    if (mode == FireWater)
    {

    }
    else if (mode == JustFloat)
    {
        Set_JustFloat_Data(buffer, length);
    }
    else if (mode == RawData)
    {

    }
}

/**
 * @brief 获取命令类型
 *
 */
Enum_VOFA_Receive_CMD_TYPE DVC_VOFA::Get_Cmd_Type()
{
    return cmd_type;
}

/**
 * @brief 获取命令值
 *
 */
int DVC_VOFA::Get_Cmd_Value()
{
    return cmd_value;
}

/**
 * @brief 接收数据处理解析函数，反馈格式为“cmd_type=cmd_value%f”
 *
 */
void DVC_VOFA::Handle_Receive_Data(const Struct_UART_Receive_Management& Receive_Management)
{
    Receive_Cmd_Type(Receive_Management);
    Receive_Cmd_Value(Receive_Management);
}

/**
 * @brief 解析VOFA反馈命令类型
 *
 */
void DVC_VOFA::Receive_Cmd_Type(const Struct_UART_Receive_Management& Receive_Management)
{
    if (Receive_Management.rx_buffer[0] == 'O')
    {
        switch (Receive_Management.rx_buffer[1])
        {
        case 'P':
            cmd_type = OP;
            break;
        case 'I':
            cmd_type = OI;
            break;
        case 'D':
            cmd_type = OD;
            break;
        case 'F':
            cmd_type = OF;
            break;
        case 'T':
            cmd_type = OT;
            break;
        }
    }
    else if (Receive_Management.rx_buffer[0] == 'A')
    {
        switch (Receive_Management.rx_buffer[1])
        {
        case 'P':
            cmd_type = AP;
            break;
        case 'I':
            cmd_type = AI;
            break;
        case 'D':
            cmd_type = AD;
            break;
        case 'F':
            cmd_type = AF;
            break;
        case 'T':
            cmd_type = AT;
            break;
        }
    }
}

/**
 * @brief 解析VOFA反馈命令值
 *
 */
void DVC_VOFA::Receive_Cmd_Value(const Struct_UART_Receive_Management& Receive_Management)
{
    cmd_value = 0;

    for (uint16_t i = 3; i < Receive_Management.rx_length; ++i)
    {
        cmd_value = cmd_value * 10 + (Receive_Management.rx_buffer[i] - '0');
    }

}
