#include "bsp_usart.h"

void BSP_USART::Init(UART_HandleTypeDef *huart, UART_Call_Back callback_function, uint16_t rx_buffer_length)
{
    UART_Handler = huart;
    UART_Callback_Function = callback_function;
    Rx_Buffer_Length = rx_buffer_length;

    HAL_UARTEx_ReceiveToIdle_DMA(huart, Rx_Buffer, Rx_Buffer_Length);
}

uint8_t BSP_USART::Send_Data(UART_HandleTypeDef* huart, uint8_t* Data, uint16_t Length)
{
    HAL_UART_Transmit_DMA(huart, Data, Length);
}
