#ifndef BSP_CAN_H
#define BSP_CAN_H

#include "fdcan.h"
#include <cstring>

/**
 * @brief CAN通信类型
 *
 */
enum CAN_Type
{
    ClassicCAN = 0,
    FDCAN_With_BitRate_Switch = 1,
    FDCAN_Without_BitRate_Switch = 2,
};

/**
 * @brief CAN接收的信息结构体
 *
 */
struct Struct_FDCAN_Rx_FIFO
{
    FDCAN_RxHeaderTypeDef Header;
    uint8_t Data[8];
};

/**
 * @brief CAN通信接收回调函数数据类型
 *
 */
typedef void (*CAN_Call_Back)(Struct_FDCAN_Rx_FIFO* );

class BSP_CAN
{
public:

    void Init(FDCAN_HandleTypeDef* hfdcan, CAN_Call_Back Callback_Function);
    uint8_t Send_Data(FDCAN_HandleTypeDef *hfdcan, uint16_t ID, uint8_t *Data, uint16_t Length);

    void Set_CAN_Type(CAN_Type can_type);

private:

    void Filter_Config(FDCAN_HandleTypeDef* hfdcan, uint32_t id_type, uint32_t filter_index, uint32_t filter_type, uint32_t filter_config, uint32_t filter_id1, int32_t filter_id2);

    CAN_Type CAN_Type_Used;
};



#endif