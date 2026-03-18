#include "bsp_usart.h"

// 这行代码是定义，它在全局作用域为object_map分配内存（并调用默认构造函数）。如果没有这行定义，链接器会报“未定义的引用”错误。
std::map<UART_HandleTypeDef*, BSP_USART*> BSP_USART::object_map;

std::shared_ptr<BSP_USART> usart1 = std::make_shared<BSP_USART>(&huart1,MAX_DATA_LENGTH);
std::shared_ptr<BSP_USART> uart5 = std::make_shared<BSP_USART>(&huart5,MAX_DATA_LENGTH);
std::shared_ptr<BSP_USART> uart7 = std::make_shared<BSP_USART>(&huart7,MAX_DATA_LENGTH);
std::shared_ptr<BSP_USART> usart10 = std::make_shared<BSP_USART>(&huart10,MAX_DATA_LENGTH);

/**
 * @brief 构造函数，传入串口句柄和接收缓冲区大小
 *
 */
BSP_USART::BSP_USART(UART_HandleTypeDef* huart_, uint16_t rx_buffer_size_)
    : huart(huart_), rx_buffer_size(rx_buffer_size_),
      rx_callback(nullptr), tx_filler(nullptr)
{
    // 分配接收缓冲区
    rx_buffer = new uint8_t[rx_buffer_size];
    // 注册到映射表
    object_map[huart] = this;
}

/**
 * @brief 静态方法，根据huart获取对象（用于中断）
 *
 */
BSP_USART* BSP_USART::Get_UART_Object(UART_HandleTypeDef* huart_)
{
    auto it = object_map.find(huart_);
    return (it != object_map.end()) ? it->second : nullptr;
}

/**
 * @brief 注册接收回调
 *
 */
void BSP_USART::Register_ReceiveCallback(UART_ReceiveCallback rx_callback_)
{
    rx_callback = rx_callback_;
}

/**
 * @brief 注册发送填充回调
 *
 */
void BSP_USART::Register_TransmitFiller(UART_TransmitFiller tx_filler_)
{
    tx_filler = tx_filler_;
}

/**
 * @brief 启动接收（使能空闲中断）
 *
 */
void BSP_USART::Start_Receive()
{
    // 停止可能的DMA传输
    HAL_UART_DMAStop(huart);
    // 启动DMA接收，最大长度
    HAL_UART_Receive_DMA(huart, rx_buffer, rx_buffer_size);
    // 使能空闲中断
    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
}

/**
 * @brief 发送数据（直接发送）
 *
 */
void BSP_USART::Send_Data(uint8_t* Data, uint16_t Length)
{
    HAL_UART_Transmit_DMA(huart, Data, Length);
}

/**
 * @brief 串口发送任务（可在任务中周期调用）
 *
 */
void BSP_USART::UART_Transmit_Task()
{
    if (tx_filler != nullptr)
    {
        uint8_t buffer[MAX_DATA_LENGTH];  // 临时缓冲区，大小可根据需要调整
        uint16_t length = 0;
        tx_filler(buffer, &length);
        if (length > 0)
        {
            Send_Data(buffer, length);
        }
    }
}

/**
 * @brief 空闲中断处理函数（由外部IRQ调用，不定长接收完成）
 *
 */
void BSP_USART::Handle_RxIdle()
{
    // 1. 清除空闲中断标志
    __HAL_UART_CLEAR_IDLEFLAG(huart);

    // 2. 停止DMA接收，防止继续写入
    HAL_UART_DMAStop(huart);

    // 3. 计算实际接收长度
    uint16_t remaining = __HAL_DMA_GET_COUNTER(huart->hdmarx);
    uint16_t received = rx_buffer_size - remaining;

    // 4. 调用接收回调（如果有）
    if (rx_callback && received > 0) {
        Struct_UART_Receive_Management mgmt;
        mgmt.rx_buffer = rx_buffer;
        mgmt.rx_length = received;
        rx_callback(mgmt);
    }

    // 5. 重新启动接收，准备下一帧
    Start_Receive();
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    BSP_USART* uart;
    uart = BSP_USART::Get_UART_Object(huart);
    uart->Handle_RxIdle();
}