#include "bsp_can.h"

/**
 * @brief 配置CAN滤波器
 *
 * @param hfdcan        CAN句柄
 * @param id_type       id类型，标准帧还是扩展帧
 * @param filter_index  选哪个滤波器
 * @param filter_type   滤波器模式，如mask或者list
 * @param filter_config 滤波器配置，如绑定给谁
 * @param filter_id1    接收ID1
 * @param filter_id2    接收ID2
 */
void BSP_CAN::Filter_Config(FDCAN_HandleTypeDef* hfdcan, uint32_t id_type, uint32_t filter_index, uint32_t filter_type, uint32_t filter_config, uint32_t filter_id1, int32_t filter_id2)
{
    FDCAN_FilterTypeDef filter;

    filter.IdType = id_type;
    filter.FilterIndex = filter_index;
    filter.FilterType = filter_type;
    filter.FilterConfig = filter_config;
    filter.FilterID1 = filter_id1;
    filter.FilterID2 = filter_id2;

    // 配置具体的单个过滤器元素，即向 FDCAN 过滤器列表中添加一条过滤规则（例如：标准 ID 或扩展 ID 的掩码模式、列表模式等）
    HAL_FDCAN_ConfigFilter(hfdcan, &filter);

    // 配置全局过滤器，即定义那些没有匹配到任何已配置的特定 ID 过滤器的帧的处理方式，以及远程帧的默认处理方式
    HAL_FDCAN_ConfigGlobalFilter(hfdcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
}

/**
 * @brief 初始化CAN
 *
 * @param hfdcan CAN句柄
 */
void BSP_CAN::Init(FDCAN_HandleTypeDef* hfdcan, CAN_Call_Back Callback_Function)
{
    HAL_FDCAN_Start(hfdcan);

    //打开FIFO0区的新数据接收中断
    HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    //打开FIFO1区的新数据接收中断
    HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);

    if (hfdcan->Instance == FDCAN1)
    {
        // 配置滤波器
        // 配置滤波器0，标准帧，mask模式，绑定到FIFO0，ID1为希望接收的ID，ID2为掩码
        Filter_Config(hfdcan, FDCAN_STANDARD_ID, 0, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO0, 0x000, 0x000);
        // 配置滤波器1，标准帧，mask模式，绑定到FIFO1，ID1为希望接收的ID，ID2为掩码
        Filter_Config(hfdcan, FDCAN_STANDARD_ID, 1, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO1, 0x000, 0x000);
    }
    else if (hfdcan->Instance == FDCAN2)
    {
        // 配置滤波器
        // 配置滤波器14，标准帧，mask模式，绑定到FIFO0，ID1为希望接收的ID，ID2为掩码
        Filter_Config(hfdcan, FDCAN_STANDARD_ID, 14, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO0, 0x000, 0x000);
        // 配置滤波器15，标准帧，mask模式，绑定到FIFO1，ID1为希望接收的ID，ID2为掩码
        Filter_Config(hfdcan, FDCAN_STANDARD_ID, 15, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO1, 0x000, 0x000);
    }
    else if (hfdcan->Instance == FDCAN3)
    {
        // 配置滤波器
        // 配置滤波器28，标准帧，mask模式，绑定到FIFO0，ID1为希望接收的ID，ID2为掩码
        Filter_Config(hfdcan, FDCAN_STANDARD_ID, 28, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO0, 0x000, 0x000);
        // 配置滤波器29，标准帧，mask模式，绑定到FIFO1，ID1为希望接收的ID，ID2为掩码
        Filter_Config(hfdcan, FDCAN_STANDARD_ID, 29, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO1, 0x000, 0x000);
    }
}

/**
 * @brief 发送数据帧
 *
 * @param hfdcan FDCAN编号
 * @param ID ID
 * @param Data 被发送的数据指针
 * @param Length 长度
 *
 * @return uint8_t 执行状态
 */
uint8_t BSP_CAN::Send_Data(FDCAN_HandleTypeDef *hfdcan, uint16_t ID, uint8_t *Data, uint16_t Length)
{
    FDCAN_TxHeaderTypeDef tx_header;

    //检测传参是否正确
    assert_param(hfdcan != nullptr);

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

    return (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &tx_header, Data));
}

// /**
//  * @brief HAL库FDCAN接收FIFO0中断
//  *
//  * @param hfdcan FDCAN编号
//  */
// void HAL_FDCAN_RxFifo0MsgPendingCallback(FDCAN_HandleTypeDef *hfdcan)
// {
//
//     // 选择回调函数
//     if (hfdcan->Instance == FDCAN1)
//     {
//         HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN1_Manage_Object.Rx_FIFO.Header, FDCAN1_Manage_Object.Rx_FIFO.Data);
//         if(FDCAN1_Manage_Object.Callback_Function != nullptr)
//         {
//             FDCAN1_Manage_Object.Callback_Function(&FDCAN1_Manage_Object.Rx_FIFO);
//         }
//     }
//     else if (hfdcan->Instance == FDCAN2)
//     {
//         HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN2_Manage_Object.Rx_FIFO.Header, FDCAN2_Manage_Object.Rx_FIFO.Data);
//         if(FDCAN2_Manage_Object.Callback_Function != nullptr)
//         {
//             FDCAN2_Manage_Object.Callback_Function(&FDCAN2_Manage_Object.Rx_FIFO);
//         }
//     }
//     else if (hfdcan->Instance == FDCAN3)
//     {
//         HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN3_Manage_Object.Rx_FIFO.Header, FDCAN3_Manage_Object.Rx_FIFO.Data);
//         if(FDCAN3_Manage_Object.Callback_Function != nullptr)
//         {
//             FDCAN3_Manage_Object.Callback_Function(&FDCAN3_Manage_Object.Rx_FIFO);
//         }
//     }
// }
//
// /**
//  * @brief HAL库FDCAN接收FIFO1中断
//  *
//  * @param hfdcan FDCAN编号
//  */
// void HAL_FDCAN_RxFifo1MsgPendingCallback(FDCAN_HandleTypeDef *hfdcan)
// {
//
//     // 选择回调函数
//     if (hfdcan->Instance == FDCAN1)
//     {
//         HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &FDCAN1_Manage_Object.Rx_FIFO.Header, FDCAN1_Manage_Object.Rx_FIFO.Data);
//         if(FDCAN1_Manage_Object.Callback_Function != nullptr)
//         {
//             FDCAN1_Manage_Object.Callback_Function(&FDCAN1_Manage_Object.Rx_FIFO);
//         }
//     }
//     else if (hfdcan->Instance == FDCAN2)
//     {
//         HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &FDCAN2_Manage_Object.Rx_FIFO.Header, FDCAN2_Manage_Object.Rx_FIFO.Data);
//         if(FDCAN2_Manage_Object.Callback_Function != nullptr)
//         {
//             FDCAN2_Manage_Object.Callback_Function(&FDCAN2_Manage_Object.Rx_FIFO);
//         }
//     }
//     else if (hfdcan->Instance == FDCAN3)
//     {
//         HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &FDCAN3_Manage_Object.Rx_FIFO.Header, FDCAN3_Manage_Object.Rx_FIFO.Data);
//         if(FDCAN3_Manage_Object.Callback_Function != nullptr)
//         {
//             FDCAN3_Manage_Object.Callback_Function(&FDCAN3_Manage_Object.Rx_FIFO);
//         }
//     }
// }

/**
 * @brief 设定使用的CAN类型
 *
 */
void BSP_CAN::Set_CAN_Type(CAN_Type can_type)
{
    CAN_Type_Used = can_type;
}
