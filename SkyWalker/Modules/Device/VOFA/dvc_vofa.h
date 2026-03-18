#ifndef DVC_VOFA_H
#define DVC_VOFA_H

#include "bsp_usart.h"

#include <memory>
#include <vector>

enum Enum_VOFA_Mode
{
    FireWater = 0,
    JustFloat,
    RawData
};

enum Enum_VOFA_Receive_CMD_TYPE
{
    OP = 0,
    OI,
    OD,
    OF,
    OT,
    AP,
    AI,
    AD,
    AF,
    AT
};

class DVC_VOFA
{
public:

    DVC_VOFA(std::shared_ptr<BSP_USART> uart_, Enum_VOFA_Mode mode_);

    void Handle_Transmit_Data(uint8_t* buffer, uint16_t* length);
    void Handle_Receive_Data(const Struct_UART_Receive_Management& Receive_Management);

    void Set_Tx_Data(const std::vector<float>& data);
    //void Set_FireWater_Data(uint8_t* buffer, uint16_t* length);
    void Set_JustFloat_Data(uint8_t* buffer, uint16_t* length);
    //void Set_RawData_Data(uint8_t* buffer, uint16_t* length);

    Enum_VOFA_Receive_CMD_TYPE Get_Cmd_Type();
    int Get_Cmd_Value();

private:

    void Receive_Cmd_Type(const Struct_UART_Receive_Management& Receive_Management);
    void Receive_Cmd_Value(const Struct_UART_Receive_Management& Receive_Management);


    // VOFA的串口通信对象
    std::shared_ptr<BSP_USART> uart;

    // 当前VOFA协议模式
    Enum_VOFA_Mode mode;

    // 要通过VOFA发送的数据
    std::vector<float> tx_data;

    // 反馈的参数类型
    Enum_VOFA_Receive_CMD_TYPE cmd_type;
    // 反馈的参数值
    int cmd_value;

};



#endif