#ifndef BSP_CAN_H
#define BSP_CAN_H

#include "fdcan.h"
#include <cstring>
#include <memory>
#include <functional>
#include <map>

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
 * @brief 定义CAN的发送填充回调函数类型
 *
 */
using FDCAN_Transmit_Filler_Callback = std::function<void(uint8_t*)>;

/**
 * @brief CAN的发送管理结构体
 *
 */
struct Struct_FDCAN_Transmit_Management
{
    // 数据在帧中的起始位置
    uint8_t offset;
    // 填充函数
    FDCAN_Transmit_Filler_Callback callback;
};

/**
 * @brief CAN的接收管理结构体
 *
 */
struct Struct_FDCAN_Receive_Management
{
    // CAN接收ID
    uint32_t ID;
    // CAN接收缓冲区
    uint8_t rx_data[8];
    // 数据长度
    uint32_t data_length;
};

/**
 * @brief 定义CAN的接收回调函数类型，此处使用常量引用，const防止接收管理结构体被修改，引用没有拷贝开销，传递量小
 *
 */
using FDCAN_ReceiveCallback = std::function<void(const Struct_FDCAN_Receive_Management&)>;


class BSP_CAN
{
public:

    BSP_CAN(FDCAN_HandleTypeDef* hfdcan, CAN_Type can_type);

    void Begin();

    void FDCAN_Transmit_Task();

    uint8_t Send_Data(uint16_t ID, uint8_t *Data, uint32_t Length);

    void Register_TransmitCallback(uint16_t tx_id, uint8_t offset, FDCAN_Transmit_Filler_Callback callback);
    void Register_ReceiveCallback(uint32_t rx_id, FDCAN_ReceiveCallback callback);

    void FIFO0_Rx_Callback();
    void FIFO1_Rx_Callback();

private:

    void Set_CAN_Type(CAN_Type can_type);

    void Filter_Config(uint32_t id_type, uint32_t filter_index, uint32_t filter_type, uint32_t filter_config, uint32_t filter_id1, int32_t filter_id2);

    // CAN通信句柄
    FDCAN_HandleTypeDef* hfdcan_;

    // CAN通信类型
    CAN_Type CAN_Type_Used;

    // 发送填充回调函数，multimap允许一个键对应多个值
    std::multimap<uint16_t, Struct_FDCAN_Transmit_Management> transmit_map;
    // 接收回调函数映射表，将ID与回调函数绑定，map只能一个键和一个值对应
    std::map<uint32_t, FDCAN_ReceiveCallback> receive_map;
};

extern std::shared_ptr<BSP_CAN> fdcan1;
extern std::shared_ptr<BSP_CAN> fdcan2;
extern std::shared_ptr<BSP_CAN> fdcan3;

#endif