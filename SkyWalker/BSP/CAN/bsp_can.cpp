#include "bsp_can.h"

// 实例化CAN类
std::shared_ptr<BSP_CAN> fdcan1 = std::make_shared<BSP_CAN>(&hfdcan1,ClassicCAN);
std::shared_ptr<BSP_CAN> fdcan2 = std::make_shared<BSP_CAN>(&hfdcan2, ClassicCAN);
std::shared_ptr<BSP_CAN> fdcan3 = std::make_shared<BSP_CAN>(&hfdcan3, ClassicCAN);

/**
 * @brief CAN类的构造函数
 *
 */
BSP_CAN::BSP_CAN(FDCAN_HandleTypeDef* hfdcan, CAN_Type can_type)
{
    hfdcan_ = hfdcan;
    Set_CAN_Type(can_type);
}

/**
 * @brief 由于涉及HAL中开启FDCAN通信等操作，此部分不可以在构造函数中完成，
 * 因为设定fdcan变量为全局变量，其构造函数在main函数之前就已经执行完成，
 * 此时MX_FDCAN_Init()还未执行，故而另起新函数
 */
void BSP_CAN::Begin()
{
    HAL_FDCAN_Start(hfdcan_);

    //打开FIFO0区的新数据接收中断，FIFO模式不涉及具体缓冲区索引，所以bufferIndexes设0
    HAL_FDCAN_ActivateNotification(hfdcan_, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    //打开FIFO1区的新数据接收中断，FIFO模式不涉及具体缓冲区索引，所以bufferIndexes设0
    HAL_FDCAN_ActivateNotification(hfdcan_, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);

    if (hfdcan_->Instance == FDCAN1)
    {
        // 配置滤波器
        // 配置滤波器0，标准帧，mask模式，绑定到FIFO0，ID1为希望接收的ID，ID2为掩码
        Filter_Config(FDCAN_STANDARD_ID, 0, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO0, 0x000, 0x000);
        // 配置滤波器1，标准帧，mask模式，绑定到FIFO1，ID1为希望接收的ID，ID2为掩码
        Filter_Config(FDCAN_STANDARD_ID, 1, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO1, 0x000, 0x000);
    }
    else if (hfdcan_->Instance == FDCAN2)
    {
        // 配置滤波器
        // 配置滤波器0，标准帧，mask模式，绑定到FIFO0，ID1为希望接收的ID，ID2为掩码
        Filter_Config(FDCAN_STANDARD_ID, 0, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO0, 0x000, 0x000);
        // 配置滤波器1，标准帧，mask模式，绑定到FIFO1，ID1为希望接收的ID，ID2为掩码
        Filter_Config(FDCAN_STANDARD_ID, 1, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO1, 0x000, 0x000);
    }
    else if (hfdcan_->Instance == FDCAN3)
    {
        // 配置滤波器
        // 配置滤波器0，标准帧，mask模式，绑定到FIFO0，ID1为希望接收的ID，ID2为掩码
        Filter_Config(FDCAN_STANDARD_ID, 0, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO0, 0x000, 0x000);
        // 配置滤波器1，标准帧，mask模式，绑定到FIFO1，ID1为希望接收的ID，ID2为掩码
        Filter_Config(FDCAN_STANDARD_ID, 1, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO1, 0x000, 0x000);
    }
}

/**
 * @brief CAN的发送任务，在skywalker_task.cpp中由FreeRTOS调用
 *
 */
void BSP_CAN::FDCAN_Transmit_Task()
{
    // 当前选择的发送ID
    uint16_t currentId = 0;
    // 要发送的数据段
    uint8_t frameData[8] = {0};

    for (auto it = transmit_map.begin(); it != transmit_map.end(); ++it)
    {
        // 当遍历的迭代器指向的条目ID与当前正在处理的ID不同时，说明已经遇到了一个新发送ID。
        // 此时需要先将上一个ID已收集的数据发送出去（如果上一个ID有效），然后切换到新的ID。
        if (it->first != currentId)
        {
            if (currentId != 0)
            {
                Send_Data(currentId, frameData, 8);
            }
            currentId = it->first;
            memset(frameData, 0, 8); // 清空缓冲区准备接收新数据
        }
        // 此之后执行传入的回调函数作为函数实体
        it->second.callback(frameData + it->second.offset);
    }

    if (currentId != 0)
    {
        Send_Data(currentId, frameData, 8);
    }
}

/**
 * @brief 配置CAN滤波器
 *
 * @param id_type       id类型，标准帧还是扩展帧
 * @param filter_index  选哪个滤波器
 * @param filter_type   滤波器模式，如mask或者list
 * @param filter_config 滤波器配置，如绑定给谁
 * @param filter_id1    接收ID1
 * @param filter_id2    接收ID2
 */
void BSP_CAN::Filter_Config(uint32_t id_type, uint32_t filter_index, uint32_t filter_type, uint32_t filter_config, uint32_t filter_id1, int32_t filter_id2)
{
    FDCAN_FilterTypeDef filter;

    filter.IdType = id_type;
    filter.FilterIndex = filter_index;
    filter.FilterType = filter_type;
    filter.FilterConfig = filter_config;
    filter.FilterID1 = filter_id1;
    filter.FilterID2 = filter_id2;

    // 配置具体的单个过滤器元素，即向 FDCAN 过滤器列表中添加一条过滤规则（例如：标准 ID 或扩展 ID 的掩码模式、列表模式等）
    HAL_FDCAN_ConfigFilter(hfdcan_, &filter);

    // 配置全局过滤器，即定义那些没有匹配到任何已配置的特定 ID 过滤器的帧的处理方式，以及远程帧的默认处理方式
    HAL_FDCAN_ConfigGlobalFilter(hfdcan_, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
}


/**
 * @brief 发送数据帧
 *
 * @param ID ID
 * @param Data 被发送的数据指针
 * @param Length 长度
 *
 * @return uint8_t 执行状态
 */
uint8_t BSP_CAN::Send_Data(uint16_t ID, uint8_t *Data, uint32_t Length)
{
    FDCAN_TxHeaderTypeDef tx_header;

    //检测传参是否正确
    assert_param(hfdcan_ != nullptr);

    tx_header.Identifier = ID; // ID
    tx_header.IdType = FDCAN_STANDARD_ID; // ID类型
    tx_header.TxFrameType = FDCAN_DATA_FRAME; // 数据帧
    tx_header.DataLength = Length; // 数据长度
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE; // 错误状态指示器

    if (CAN_Type_Used == ClassicCAN)
    {
        tx_header.FDFormat = FDCAN_CLASSIC_CAN; // 经典CAN格式
        tx_header.BitRateSwitch = FDCAN_BRS_OFF; // 速率切换
    }
    else if (CAN_Type_Used == FDCAN_Without_BitRate_Switch)
    {
        tx_header.FDFormat = FDCAN_FD_CAN; // FDCAN格式
        tx_header.BitRateSwitch = FDCAN_BRS_OFF; // 速率切换
    }
    else if (CAN_Type_Used == FDCAN_With_BitRate_Switch)
    {
        tx_header.FDFormat = FDCAN_FD_CAN; // FDCAN格式
        tx_header.BitRateSwitch = FDCAN_BRS_ON; // 速率切换
    }

    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // 事件FIFO控制
    tx_header.MessageMarker = 0; // 消息标记

    return (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan_, &tx_header, Data));
}

/**
 * @brief 设定使用的CAN类型
 *
 */
void BSP_CAN::Set_CAN_Type(CAN_Type can_type)
{
    CAN_Type_Used = can_type;
}

/**
 * @brief 发送回调函数注册接口，供外部调用
 *
 * @param tx_id      发送报文id
 * @param offset     当前要发送的id在报文中的位置
 * @param callback   发送填充回调函数
 *
 */
void BSP_CAN::Register_TransmitCallback(uint16_t tx_id, uint8_t offset, FDCAN_Transmit_Filler_Callback callback)
{
    // std::multimap的元素类型是std::pair<const Key, Value>。使用emplace时，你只需要提供构造Key和Value所需的参数，emplace会在容器内部就地构造一个pair对象，然后将其插入到map中。
    transmit_map.emplace(tx_id, Struct_FDCAN_Transmit_Management{offset, callback});
}

/**
 * @brief 接收回调函数注册接口，将ID和回调函数在映射表中绑定，供外部调用
 *
 * @param rx_id       反馈报文ID
 * @param callback    反馈回调函数
 */
void BSP_CAN::Register_ReceiveCallback(uint32_t rx_id, FDCAN_ReceiveCallback callback)
{
    receive_map[rx_id] = callback;
}

/**
 * @brief FIFO0接收回调函数
 *
 */
void BSP_CAN::FIFO0_Rx_Callback()
{
    FDCAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    // 从 FIFO0读取一帧
    if (HAL_FDCAN_GetRxMessage(hfdcan_, FDCAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
    {
        uint32_t id = rxHeader.Identifier;
        // 在映射表中查找该 ID 对应的回调
        auto it = receive_map.find(id);
        if (it != receive_map.end())
        {
            // 构造数据对象
            Struct_FDCAN_Receive_Management FDCAN_Receive_Management;
            FDCAN_Receive_Management.ID = id;
            // 数据长度拷贝
            FDCAN_Receive_Management.data_length = rxHeader.DataLength;
            memcpy(FDCAN_Receive_Management.rx_data, rxData, FDCAN_Receive_Management.data_length);

            // 调用回调函数，将数据传给它。it->first就是那个ID本身，it->second就是与该ID绑定的回调函数。
            it->second(FDCAN_Receive_Management);
        }
    }
}

/**
 * @brief FIFO1接收回调函数
 *
 */
void BSP_CAN::FIFO1_Rx_Callback()
{
    FDCAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    // 从 FIFO1读取一帧
    if (HAL_FDCAN_GetRxMessage(hfdcan_, FDCAN_RX_FIFO1, &rxHeader, rxData) == HAL_OK)
    {
        uint32_t id = rxHeader.Identifier;
        // 在映射表中查找该 ID 对应的回调
        auto it = receive_map.find(id);
        if (it != receive_map.end())
        {
            // 构造数据对象
            Struct_FDCAN_Receive_Management FDCAN_Receive_Management;
            FDCAN_Receive_Management.ID = id;
            // 数据长度拷贝
            FDCAN_Receive_Management.data_length = rxHeader.DataLength;
            memcpy(FDCAN_Receive_Management.rx_data, rxData, FDCAN_Receive_Management.data_length);

            // 调用回调函数，将数据传给它。it->first就是那个ID本身，it->second就是与该ID绑定的回调函数。
            it->second(FDCAN_Receive_Management);
        }
    }
}

/**
 * @brief HAL库FDCAN接收FIFO0中断
 *
 * @param hfdcan FDCAN编号
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if (hfdcan->Instance == FDCAN1)
    {
        //fdcan1->FIFO0_Rx_Callback();
    }
    else if (hfdcan->Instance == FDCAN2)
    {
        fdcan2->FIFO0_Rx_Callback();
    }
    else if (hfdcan->Instance == FDCAN3)
    {
        fdcan3->FIFO0_Rx_Callback();
    }
}

/**
 * @brief HAL库FDCAN接收FIFO1中断
 *
 * @param hfdcan FDCAN编号
 */
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if (hfdcan->Instance == FDCAN1)
    {
        //fdcan1->FIFO1_Rx_Callback();
    }
    else if (hfdcan->Instance == FDCAN2)
    {
        fdcan2->FIFO1_Rx_Callback();
    }
    else if (hfdcan->Instance == FDCAN3)
    {
        fdcan3->FIFO1_Rx_Callback();
    }
}

