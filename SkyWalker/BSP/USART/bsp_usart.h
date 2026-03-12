#ifndef BSP_USART_H
#define BSP_USART_H

#include "usart.h"
#include "dma.h"

// 缓冲区字节长度
#define UART_BUFFER_SIZE 512

/**
 * @brief UART通信接收回调函数数据类型
 *
 */
typedef void (*UART_Call_Back)(uint8_t *Buffer, uint16_t Length);

class BSP_USART
{
public:

    void Init(UART_HandleTypeDef *huart, UART_Call_Back callback_function, uint16_t rx_buffer_length);
    uint8_t Send_Data(UART_HandleTypeDef *huart, uint8_t *Data, uint16_t Length);

private:

    UART_HandleTypeDef *UART_Handler;

    uint8_t Rx_Buffer[UART_BUFFER_SIZE];
    //uint8_t Tx_Buffer[UART_BUFFER_SIZE];

    uint16_t Rx_Buffer_Length;

    UART_Call_Back UART_Callback_Function;
};




#endif
