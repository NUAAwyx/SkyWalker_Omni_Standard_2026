#ifndef BSP_USART_H
#define BSP_USART_H

#include "usart.h"
#include "dma.h"

#include <map>
#include <functional>
#include <cstring>
#include <memory>

#define MAX_DATA_LENGTH 512

// 接收信息结构体（传递给上层回调）
struct Struct_UART_Receive_Management
{
    uint8_t* rx_buffer;   // 指向接收缓冲区
    uint16_t rx_length;   // 实际接收长度
};

// 接收回调类型
using UART_ReceiveCallback = std::function<void(const Struct_UART_Receive_Management&)>;
// 发送填充回调类型：用于构造要发送的数据
using UART_TransmitFiller = std::function<void(uint8_t* buffer, uint16_t* length)>;

class BSP_USART
{
public:

    BSP_USART(UART_HandleTypeDef* huart_, uint16_t rx_buffer_size);

    static BSP_USART* Get_UART_Object(UART_HandleTypeDef* huart_);

    void Register_ReceiveCallback(UART_ReceiveCallback rx_callback_);
    void Register_TransmitFiller(UART_TransmitFiller tx_filler_);

    void Start_Receive();

    void Send_Data(uint8_t* Data, uint16_t Length);

    void UART_Transmit_Task();

    void Handle_RxIdle();

private:

    UART_HandleTypeDef* huart;
    uint16_t rx_buffer_size;    // 缓冲区大小
    uint8_t* rx_buffer;         // DMA接收缓冲区

    UART_ReceiveCallback rx_callback;   // 接收回调
    UART_TransmitFiller tx_filler;       // 发送填充回调

    // 静态映射表：huart -> BSP_USART*
    static std::map<UART_HandleTypeDef*, BSP_USART*> object_map;
};

extern std::shared_ptr<BSP_USART> usart1;   // 裁判系统串口
extern std::shared_ptr<BSP_USART> uart5;    // DR16串口
extern std::shared_ptr<BSP_USART> uart7;    // NUC通信串口
extern std::shared_ptr<BSP_USART> usart10;  // VOFA串口

#endif