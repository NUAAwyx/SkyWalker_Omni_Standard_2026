#ifndef DVC_REFEREE_H
#define DVC_REFEREE_H

#include "bsp_usart.h"

// 机器人类型选择（仅开启一个）
// #define Robot_HERO_1
// #define Robot_ENGINEER_2
#define Robot_INFANTRY_3
// #define Robot_INFANTRY_4
// #define Robot_INFANTRY_5
// #define Robot_AERIAL_6
// #define Robot_SENTRY_7
// #define Robot_DART_8
// #define Robot_RADAR_9
// #define Robot_BASE_10
// #define Robot_OUTPOST_11

/* Exported types ------------------------------------------------------------*/
/**
 * @brief 裁判系统工作状态
 */
enum Enum_Referee_Status : uint8_t
{
    Referee_Status_DISABLE = 0,
    Referee_Status_ENABLE,
};

/**
 * @brief 通用使能/失能状态
 */
enum Enum_Referee_Data_Status : uint8_t
{
    Referee_Data_Status_DISABLE = 0,
    Referee_Data_Status_ENABLE,
};

/**
 * @brief 裁判系统命令码ID (RM2026 V1.2.0)
 */
enum Enum_Referee_Command_ID : uint16_t
{
    // 比赛状态相关
    Referee_Command_ID_GAME_STATUS          = 0x0001,  // 比赛状态数据 1Hz
    Referee_Command_ID_GAME_RESULT          = 0x0002,  // 比赛结果数据 结束触发
    Referee_Command_ID_GAME_ROBOT_HP        = 0x0003,  // 机器人血量数据 3Hz
    // 场地事件相关
    Referee_Command_ID_EVENT_DATA           = 0x0101,  // 场地事件数据 1Hz
    Referee_Command_ID_EVENT_REFEREE_WARNING= 0x0104,  // 裁判警告数据 判罚触发
    Referee_Command_ID_EVENT_DART_INFO      = 0x0105,  // 飞镖发射相关数据 1Hz
    // 机器人本体相关
    Referee_Command_ID_ROBOT_STATUS         = 0x0201,  // 机器人性能体系数据 10Hz
    Referee_Command_ID_ROBOT_POWER_HEAT     = 0x0202,  // 实时功率热量数据 10Hz
    Referee_Command_ID_ROBOT_POSITION       = 0x0203,  // 机器人位置数据 1Hz
    Referee_Command_ID_ROBOT_BUFF           = 0x0204,  // 机器人增益数据 3Hz
    Referee_Command_ID_ROBOT_DAMAGE         = 0x0206,  // 伤害状态数据 伤害触发
    Referee_Command_ID_ROBOT_SHOOT          = 0x0207,  // 实时射击数据 发射触发
    Referee_Command_ID_ROBOT_AMMO_REMAIN    = 0x0208,  // 允许发弹量数据 10Hz
    Referee_Command_ID_ROBOT_RFID           = 0x0209,  // RFID模块状态 3Hz
    Referee_Command_ID_ROBOT_DART_CMD       = 0x020A,  // 飞镖选手端指令 3Hz
    Referee_Command_ID_GROUND_ROBOT_POS     = 0x020B,  // 地面机器人位置 1Hz(哨兵专属)
    Referee_Command_ID_RADAR_MARK_PROGRESS  = 0x020C,  // 雷达标记进度 1Hz(雷达专属)
    Referee_Command_ID_SENTRY_DECISION_SYNC = 0x020D,  // 哨兵自主决策同步 1Hz(哨兵专属)
    Referee_Command_ID_RADAR_DECISION_SYNC  = 0x020E,  // 雷达自主决策同步 1Hz(雷达专属)
    // 交互数据相关
    Referee_Command_ID_INTERACTION          = 0x0301,  // 机器人交互数据 30Hz上限
    Referee_Command_ID_MAP_COMMAND          = 0x0303,  // 选手端小地图交互 触发发送
    Referee_Command_ID_MAP_RADAR_DATA       = 0x0305,  // 选手端雷达位置数据 5Hz上限
    Referee_Command_ID_MAP_PATH_DATA        = 0x0307,  // 选手端路径数据 1Hz上限
    Referee_Command_ID_CUSTOM_INFO          = 0x0308,  // 选手端自定义消息 3Hz上限
};

/**
 * @brief 单方机器人ID
 */
enum Enum_Referee_Data_Robot_ID : uint8_t
{
    Referee_Data_Robot_ID_NULL        = 0,
    Referee_Data_Robot_ID_HERO_1      = 1,
    Referee_Data_Robot_ID_ENGINEER_2  = 2,
    Referee_Data_Robot_ID_INFANTRY_3  = 3,
    Referee_Data_Robot_ID_INFANTRY_4  = 4,
    Referee_Data_Robot_ID_INFANTRY_5  = 5,
    Referee_Data_Robot_ID_AERIAL_6    = 6,
    Referee_Data_Robot_ID_SENTRY_7    = 7,
    Referee_Data_Robot_ID_DART_8      = 8,
    Referee_Data_Robot_ID_RADAR_9     = 9,
    Referee_Data_Robot_ID_OUTPOST_10  = 10,
    Referee_Data_Robot_ID_BASE_11     = 11,
};

/**
 * @brief 双方机器人完整ID (RM2026附录二)
 */
enum Enum_Referee_Data_Robots_ID : uint8_t
{
    Referee_Data_Robots_ID_NO                = 0,
    // 红方
    Referee_Data_Robots_ID_RED_HERO_1        = 1,
    Referee_Data_Robots_ID_RED_ENGINEER_2    = 2,
    Referee_Data_Robots_ID_RED_INFANTRY_3    = 3,
    Referee_Data_Robots_ID_RED_INFANTRY_4    = 4,
    Referee_Data_Robots_ID_RED_INFANTRY_5    = 5,
    Referee_Data_Robots_ID_RED_AERIAL_6      = 6,
    Referee_Data_Robots_ID_RED_SENTRY_7      = 7,
    Referee_Data_Robots_ID_RED_DART_8        = 8,
    Referee_Data_Robots_ID_RED_RADAR_9       = 9,
    Referee_Data_Robots_ID_RED_OUTPOST_10    = 10,
    Referee_Data_Robots_ID_RED_BASE_11       = 11,
    // 蓝方
    Referee_Data_Robots_ID_BLUE_HERO_1       = 101,
    Referee_Data_Robots_ID_BLUE_ENGINEER_2   = 102,
    Referee_Data_Robots_ID_BLUE_INFANTRY_3   = 103,
    Referee_Data_Robots_ID_BLUE_INFANTRY_4   = 104,
    Referee_Data_Robots_ID_BLUE_INFANTRY_5   = 105,
    Referee_Data_Robots_ID_BLUE_AERIAL_6     = 106,
    Referee_Data_Robots_ID_BLUE_SENTRY_7     = 107,
    Referee_Data_Robots_ID_BLUE_DART_8       = 108,
    Referee_Data_Robots_ID_BLUE_RADAR_9      = 109,
    Referee_Data_Robots_ID_BLUE_OUTPOST_10   = 110,
    Referee_Data_Robots_ID_BLUE_BASE_11      = 111,
};

/**
 * @brief 选手端客户端ID
 */
enum Enum_Referee_Data_Client_ID : uint16_t
{
    Referee_Data_Client_ID_NO                = 0,
    Referee_Data_Client_ID_RED_HERO_1        = 0x0101,
    Referee_Data_Client_ID_RED_ENGINEER_2    = 0x0102,
    Referee_Data_Client_ID_RED_INFANTRY_3    = 0x0103,
    Referee_Data_Client_ID_RED_INFANTRY_4    = 0x0104,
    Referee_Data_Client_ID_RED_INFANTRY_5    = 0x0105,
    Referee_Data_Client_ID_RED_AERIAL_6      = 0x0106,
    Referee_Data_Client_ID_BLUE_HERO_1       = 0x0165,
    Referee_Data_Client_ID_BLUE_ENGINEER_2   = 0x0166,
    Referee_Data_Client_ID_BLUE_INFANTRY_3   = 0x0167,
    Referee_Data_Client_ID_BLUE_INFANTRY_4   = 0x0168,
    Referee_Data_Client_ID_BLUE_INFANTRY_5   = 0x0169,
    Referee_Data_Client_ID_BLUE_AERIAL_6     = 0x016A,
    Referee_Data_Client_ID_SERVER            = 0x8080,
};

/**
 * @brief 比赛类型
 */
enum Enum_Referee_Game_Type : uint8_t
{
    Referee_Game_Type_RMUC          = 1,
    Referee_Game_Type_RMUL_3V3      = 4,
    Referee_Game_Type_RMUL_1V1      = 5,
};

/**
 * @brief 比赛阶段
 */
enum Enum_Referee_Game_Stage : uint8_t
{
    Referee_Game_Stage_NOT_STARTED  = 0,
    Referee_Game_Stage_READY        = 1,
    Referee_Game_Stage_SELF_TEST    = 2,
    Referee_Game_Stage_5S_COUNTDOWN = 3,
    Referee_Game_Stage_BATTLE       = 4,
    Referee_Game_Stage_SETTLEMENT   = 5,
};

/**
 * @brief 比赛结果
 */
enum Enum_Referee_Game_Result : uint8_t
{
    Referee_Game_Result_DRAW        = 0,
    Referee_Game_Result_RED_WIN     = 1,
    Referee_Game_Result_BLUE_WIN    = 2,
};

/**
 * @brief 裁判警告等级
 */
enum Enum_Referee_Warning_Level : uint8_t
{
    Referee_Warning_Level_BOTH_YELLOW  = 1,
    Referee_Warning_Level_YELLOW       = 2,
    Referee_Warning_Level_RED          = 3,
    Referee_Warning_Level_FAIL         = 4,
};

/**
 * @brief 伤害类型
 */
enum Enum_Referee_Damage_Type : uint8_t
{
    Referee_Damage_Type_ARMOR_ATTACK  = 0,
    Referee_Damage_Type_MODULE_OFFLINE = 1,
    Referee_Damage_Type_COLLISION      = 5,
};

/**
 * @brief 弹丸类型
 */
enum Enum_Referee_Ammo_Type : uint8_t
{
    Referee_Ammo_Type_17MM             = 1,
    Referee_Ammo_Type_42MM             = 2,
};

/**
 * @brief 发射机构ID
 */
enum Enum_Referee_Booster_Type : uint8_t
{
    Referee_Booster_Type_17MM_1        = 1,
    Referee_Booster_Type_17MM_2        = 2,
    Referee_Booster_Type_42MM          = 3,
};

/**
 * @brief 飞镖发射站状态
 */
enum Enum_Referee_Dart_Status : uint8_t
{
    Referee_Dart_Status_OPENED         = 0,
    Referee_Dart_Status_CLOSED         = 1,
    Referee_Dart_Status_SWITCHING      = 2,
};

/**
 * @brief 飞镖击打目标
 */
enum Enum_Referee_Dart_Target : uint8_t
{
    Referee_Dart_Target_OUTPOST        = 0,
    Referee_Dart_Target_BASE_FIXED     = 1,
    Referee_Dart_Target_BASE_RAND_FIX  = 2,
    Referee_Dart_Target_BASE_RAND_MOVE = 3,
    Referee_Dart_Target_BASE_END_MOVE  = 4,
};

/**
 * @brief 哨兵姿态
 */
enum Enum_Referee_Sentry_Posture : uint8_t
{
    Referee_Sentry_Posture_ATTACK      = 1,
    Referee_Sentry_Posture_DEFEND      = 2,
    Referee_Sentry_Posture_MOVE        = 3,
};

/**
 * @brief 图形操作类型
 */
enum Enum_Graphic_Operation : uint8_t
{
    Graphic_Operation_NULL              = 0,
    Graphic_Operation_ADD               = 1,
    Graphic_Operation_MODIFY            = 2,
    Graphic_Operation_DELETE            = 3,
};

/**
 * @brief 图形类型
 */
enum Enum_Graphic_Type : uint8_t
{
    Graphic_Type_LINE                   = 0,
    Graphic_Type_RECTANGLE              = 1,
    Graphic_Type_CIRCLE                 = 2,
    Graphic_Type_OVAL                   = 3,
    Graphic_Type_ARC                    = 4,
    Graphic_Type_FLOAT                  = 5,
    Graphic_Type_INTEGER                = 6,
    Graphic_Type_STRING                 = 7,
};

/**
 * @brief 图形颜色
 */
enum Enum_Graphic_Color : uint8_t
{
    Graphic_Color_MAIN                  = 0,
    Graphic_Color_YELLOW                = 1,
    Graphic_Color_GREEN                 = 2,
    Graphic_Color_ORANGE                = 3,
    Graphic_Color_PURPLE                = 4,
    Graphic_Color_PINK                  = 5,
    Graphic_Color_CYAN                  = 6,
    Graphic_Color_BLACK                 = 7,
    Graphic_Color_WHITE                 = 8,
};

/**
 * @brief 图层删除操作
 */
enum Enum_Interaction_Delete_Op : uint8_t
{
    Interaction_Delete_Op_NULL          = 0,
    Interaction_Delete_Op_ONE           = 1,
    Interaction_Delete_Op_ALL           = 2,
};

/**
 * @brief 交互子内容ID
 */
enum Enum_Interaction_Sub_ID : uint16_t
{
    Interaction_Sub_ID_DELETE_LAYER     = 0x0100,
    Interaction_Sub_ID_GRAPHIC_1        = 0x0101,
    Interaction_Sub_ID_GRAPHIC_2        = 0x0102,
    Interaction_Sub_ID_GRAPHIC_5        = 0x0103,
    Interaction_Sub_ID_GRAPHIC_7        = 0x0104,
    Interaction_Sub_ID_STRING           = 0x0110,
    Interaction_Sub_ID_SENTRY_CMD       = 0x0120,
    Interaction_Sub_ID_RADAR_CMD        = 0x0121,
    Interaction_Sub_ID_ROBOT_COMM       = 0x0200,
};

/* 图形交互结构体定义 */
/**
 * @brief 通用图形配置结构体
 */
struct Struct_Graphic_Config
{
    uint8_t Index[3];
    uint32_t Operation_Enum : 3;
    uint32_t Type_Enum : 3;
    uint32_t Layer : 4;
    uint32_t Color_Enum : 4;
    uint32_t Details_A : 9;
    uint32_t Details_B : 9;
    uint32_t Line_Width : 10;
    uint32_t Start_X : 11;
    uint32_t Start_Y : 11;
    uint32_t Details_C : 10;
    uint32_t Details_D : 11;
    uint32_t Details_E : 11;
} __attribute__((packed));

/**
 * @brief 裁判系统串口帧结构体 (RM2026标准帧格式)
 */
struct Struct_Referee_UART_Data
{
    // 帧头 5字节
    uint8_t SOF;                          // 起始字节 固定0xA5
    uint16_t Data_Length;                 // data段长度
    uint8_t Sequence;                     // 包序号
    uint8_t CRC8;                         // 帧头CRC8校验
    // 数据段
    Enum_Referee_Command_ID Cmd_ID;       // 命令码 2字节
    uint8_t Data[128];                    // 数据段 最大128字节
    // 帧尾 2字节CRC16(整包校验)
} __attribute__((packed));

/* 接收数据结构体定义 (严格匹配RM2026协议) */
/**
 * @brief 0x0001 比赛状态数据
 */
struct Struct_Referee_Rx_Game_Status
{
    uint8_t Game_Type : 4;
    uint8_t Game_Progress : 4;
    uint16_t Stage_Remain_Time;
    uint64_t Sync_TimeStamp;
} __attribute__((packed));

/**
 * @brief 0x0002 比赛结果数据
 */
struct Struct_Referee_Rx_Game_Result
{
    uint8_t Winner;
} __attribute__((packed));

/**
 * @brief 0x0003 机器人血量数据
 */
struct Struct_Referee_Rx_Robot_HP
{
    uint16_t Red_Hero_1;
    uint16_t Red_Engineer_2;
    uint16_t Red_Infantry_3;
    uint16_t Red_Infantry_4;
    uint16_t Red_Reserved;
    uint16_t Red_Sentry_7;
    uint16_t Red_Outpost_10;
    uint16_t Red_Base_11;
    uint16_t Blue_Hero_1;
    uint16_t Blue_Engineer_2;
    uint16_t Blue_Infantry_3;
    uint16_t Blue_Infantry_4;
    uint16_t Blue_Reserved;
    uint16_t Blue_Sentry_7;
    uint16_t Blue_Outpost_10;
    uint16_t Blue_Base_11;
} __attribute__((packed));

/**
 * @brief 0x0101 场地事件数据
 */
struct Struct_Referee_Rx_Event_Data
{
    uint32_t Event_Data;
} __attribute__((packed));

/**
 * @brief 0x0104 裁判警告数据
 */
struct Struct_Referee_Rx_Warning
{
    uint8_t Level;
    uint8_t Robot_ID;
    uint8_t Count;
} __attribute__((packed));

/**
 * @brief 0x0105 飞镖信息数据
 */
struct Struct_Referee_Rx_Dart_Info
{
    uint8_t Dart_Remaining_Time;
    uint16_t Dart_Info;
} __attribute__((packed));

/**
 * @brief 0x0201 机器人性能体系数据
 */
struct Struct_Referee_Rx_Robot_Status
{
    uint8_t Robot_ID;
    uint8_t Robot_Level;
    uint16_t Current_HP;
    uint16_t Maximum_HP;
    uint16_t Cooling_Value;
    uint16_t Heat_Limit;
    uint16_t Chassis_Power_Limit;
    uint8_t Gimbal_Output : 1;
    uint8_t Chassis_Output : 1;
    uint8_t Shooter_Output : 1;
    uint8_t Reserved : 5;
} __attribute__((packed));

/**
 * @brief 0x0202 实时功率热量数据
 */
struct Struct_Referee_Rx_Power_Heat
{
    uint16_t Reserved1;
    uint16_t Reserved2;
    float Reserved3;
    uint16_t Buffer_Energy;
    uint16_t Heat_17mm;
    uint16_t Heat_42mm;
} __attribute__((packed));

/**
 * @brief 0x0203 机器人位置数据
 */
struct Struct_Referee_Rx_Robot_Position
{
    float X;
    float Y;
    float Z;
    float Yaw;
} __attribute__((packed));

/**
 * @brief 0x0204 机器人增益数据
 */
struct Struct_Referee_Rx_Robot_Buff
{
    uint8_t Recovery_Buff;
    uint16_t Cooling_Buff;
    uint8_t Defence_Buff;
    uint8_t Vulnerability_Buff;
    uint16_t Attack_Buff;
    uint8_t Remaining_Energy;
} __attribute__((packed));

/**
 * @brief 0x0206 伤害状态数据
 */
struct Struct_Referee_Rx_Damage
{
    uint8_t Armor_ID : 4;
    uint8_t Damage_Type : 4;
} __attribute__((packed));

/**
 * @brief 0x0207 实时射击数据
 */
struct Struct_Referee_Rx_Shoot
{
    uint8_t Ammo_Type;
    uint8_t Booster_ID;
    uint8_t Frequency;
    float Speed;
} __attribute__((packed));

/**
 * @brief 0x0208 允许发弹量数据
 */
struct Struct_Referee_Rx_Ammo_Remain
{
    uint16_t Ammo_17mm;
    uint16_t Ammo_42mm;
    uint16_t Gold;
    uint16_t Reserved_Ammo;
} __attribute__((packed));

/**
 * @brief 0x0209 RFID状态数据
 */
struct Struct_Referee_Rx_RFID
{
    uint32_t RFID_Status;
    uint8_t RFID_Status_2;
} __attribute__((packed));

/**
 * @brief 0x020A 飞镖选手端指令数据
 */
struct Struct_Referee_Rx_Dart_Cmd
{
    uint8_t Launch_Status;
    uint8_t Reserved;
    uint16_t Target_Change_Time;
    uint16_t Latest_Launch_Cmd_Time;
} __attribute__((packed));

/**
 * @brief 0x020B 地面机器人位置数据(哨兵专属)
 */
struct Struct_Referee_Rx_Ground_Pos
{
    float Hero_X;
    float Hero_Y;
    float Engineer_X;
    float Engineer_Y;
    float Infantry_3_X;
    float Infantry_3_Y;
    float Infantry_4_X;
    float Infantry_4_Y;
    float Reserved1;
    float Reserved2;
} __attribute__((packed));

/**
 * @brief 0x020D 哨兵自主决策同步数据
 */
struct Struct_Referee_Rx_Sentry_Sync
{
    uint32_t Sentry_Info;
    uint16_t Sentry_Status;
} __attribute__((packed));

/**
 * @brief 0x020E 雷达自主决策同步数据
 */
struct Struct_Referee_Rx_Radar_Sync
{
    uint8_t Radar_Info;
} __attribute__((packed));

/**
 * @brief 0x0303 选手端小地图指令数据
 */
struct Struct_Referee_Rx_Map_Cmd
{
    float Target_X;
    float Target_Y;
    uint8_t Cmd_Key;
    uint8_t Target_Robot_ID;
    uint16_t Cmd_Source;
} __attribute__((packed));

/**
 * @brief 0x0305 选手端雷达位置数据
 */
struct Struct_Referee_Rx_Radar_Map_Data
{
    uint16_t Hero_X;
    uint16_t Hero_Y;
    uint16_t Engineer_X;
    uint16_t Engineer_Y;
    uint16_t Infantry_3_X;
    uint16_t Infantry_3_Y;
    uint16_t Infantry_4_X;
    uint16_t Infantry_4_Y;
    uint16_t Reserved1;
    uint16_t Reserved2;
    uint16_t Sentry_X;
    uint16_t Sentry_Y;
} __attribute__((packed));

/**
 * @brief 0x0307 选手端路径数据
 */
struct Struct_Referee_Rx_Path_Data
{
    uint8_t Intention;
    uint16_t Start_X;
    uint16_t Start_Y;
    int8_t Delta_X[49];
    int8_t Delta_Y[49];
    uint16_t Sender_ID;
} __attribute__((packed));

/**
 * @brief 0x0308 选手端自定义消息
 */
struct Struct_Referee_Rx_Custom_Info
{
    uint16_t Sender_ID;
    uint16_t Receiver_ID;
    uint8_t User_Data[30];
} __attribute__((packed));

/* 发送数据结构体定义 */
/**
 * @brief 0x0301 交互数据帧头
 */
struct Struct_Referee_Tx_Interaction_Header
{
    uint16_t Data_Cmd_ID;
    uint16_t Sender_ID;
    uint16_t Receiver_ID;
} __attribute__((packed));

/**
 * @brief 0x0100 删除图层指令
 */
struct Struct_Referee_Tx_Delete_Layer
{
    Struct_Referee_Tx_Interaction_Header Header;
    uint8_t Delete_Type;
    uint8_t Layer;
} __attribute__((packed));

/**
 * @brief 0x0101 单图形绘制指令
 */
struct Struct_Referee_Tx_Graphic_1
{
    Struct_Referee_Tx_Interaction_Header Header;
    Struct_Graphic_Config Graphic;
} __attribute__((packed));

/**
 * @brief 0x0120 哨兵自主决策指令
 */
struct Struct_Referee_Tx_Sentry_Cmd
{
    Struct_Referee_Tx_Interaction_Header Header;
    uint32_t Sentry_Cmd;
} __attribute__((packed));

/**
 * @brief 0x0200 机器人间自定义通信
 */
struct Struct_Referee_Tx_Robot_Comm
{
    Struct_Referee_Tx_Interaction_Header Header;
    uint8_t User_Data[112];
} __attribute__((packed));


class DVC_REFEREE
{
public:

    DVC_REFEREE(std::shared_ptr<BSP_USART> usart_, uint8_t frame_header = 0xA5);

    void Handle_Receive_Data(const Struct_UART_Receive_Management& Receive_Management);

    // 数据获取内联函数
    inline Enum_Referee_Status Get_Referee_Status() { return Referee_Status; }
    // 比赛相关
    inline Enum_Referee_Game_Type Get_Game_Type() { return static_cast<Enum_Referee_Game_Type>(Game_Status.Game_Type); }
    inline Enum_Referee_Game_Stage Get_Game_Stage() { return static_cast<Enum_Referee_Game_Stage>(Game_Status.Game_Progress); }
    inline uint16_t Get_Stage_Remain_Time() { return Game_Status.Stage_Remain_Time; }
    inline uint64_t Get_Sync_Timestamp() { return Game_Status.Sync_TimeStamp; }
    inline Enum_Referee_Game_Result Get_Game_Result() { return static_cast<Enum_Referee_Game_Result>(Game_Result.Winner); }
    // 血量相关
    inline uint16_t Get_Robot_HP(Enum_Referee_Data_Robots_ID Robot_ID);
    // 机器人本体相关
    inline Enum_Referee_Data_Robots_ID Get_Self_ID() { return static_cast<Enum_Referee_Data_Robots_ID>(Robot_Status.Robot_ID); }
    inline uint8_t Get_Self_Level() { return Robot_Status.Robot_Level; }
    inline uint16_t Get_Self_HP() { return Robot_Status.Current_HP; }
    inline uint16_t Get_Self_HP_Max() { return Robot_Status.Maximum_HP; }
    inline uint16_t Get_Shooter_Cooling_Value() { return Robot_Status.Cooling_Value; }
    inline uint16_t Get_Shooter_Heat_Limit() { return Robot_Status.Heat_Limit; }
    inline uint16_t Get_Chassis_Power_Limit() { return Robot_Status.Chassis_Power_Limit; }
    inline Enum_Referee_Data_Status Get_Gimbal_Output_Status() { return static_cast<Enum_Referee_Data_Status>(Robot_Status.Gimbal_Output); }
    inline Enum_Referee_Data_Status Get_Chassis_Output_Status() { return static_cast<Enum_Referee_Data_Status>(Robot_Status.Chassis_Output); }
    inline Enum_Referee_Data_Status Get_Shooter_Output_Status() { return static_cast<Enum_Referee_Data_Status>(Robot_Status.Shooter_Output); }
    // 功率热量相关
    inline uint16_t Get_Chassis_Buffer_Energy() { return Power_Heat.Buffer_Energy; }
    inline uint16_t Get_17mm_Heat() { return Power_Heat.Heat_17mm; }
    inline uint16_t Get_42mm_Heat() { return Power_Heat.Heat_42mm; }
    // 位置相关
    inline float Get_Self_X() { return Robot_Position.X; }
    inline float Get_Self_Y() { return Robot_Position.Y; }
    inline float Get_Self_Z() { return Robot_Position.Z; }
    inline float Get_Self_Yaw() { return Robot_Position.Yaw; }
    // 增益相关
    inline uint8_t Get_Recovery_Buff() { return Robot_Buff.Recovery_Buff; }
    inline uint16_t Get_Cooling_Buff() { return Robot_Buff.Cooling_Buff; }
    inline uint8_t Get_Defence_Buff() { return Robot_Buff.Defence_Buff; }
    inline uint8_t Get_Vulnerability_Buff() { return Robot_Buff.Vulnerability_Buff; }
    inline uint16_t Get_Attack_Buff() { return Robot_Buff.Attack_Buff; }
    inline uint8_t Get_Remaining_Energy() { return Robot_Buff.Remaining_Energy; }
    // 伤害相关
    inline uint8_t Get_Attacked_Armor_ID() { return Damage.Armor_ID; }
    inline Enum_Referee_Damage_Type Get_Damage_Type() { return static_cast<Enum_Referee_Damage_Type>(Damage.Damage_Type); }
    // 射击相关
    inline Enum_Referee_Ammo_Type Get_Shoot_Ammo_Type() { return static_cast<Enum_Referee_Ammo_Type>(Shoot.Ammo_Type); }
    inline Enum_Referee_Booster_Type Get_Shoot_Booster_Type() { return static_cast<Enum_Referee_Booster_Type>(Shoot.Booster_ID); }
    inline uint8_t Get_Shoot_Frequency() { return Shoot.Frequency; }
    inline float Get_Shoot_Speed() { return Shoot.Speed; }
    // 弹量相关
    inline uint16_t Get_17mm_Ammo_Remain() { return Ammo_Remain.Ammo_17mm; }
    inline uint16_t Get_42mm_Ammo_Remain() { return Ammo_Remain.Ammo_42mm; }
    inline uint16_t Get_Gold_Remain() { return Ammo_Remain.Gold; }
    // RFID相关
    inline uint32_t Get_RFID_Status() { return RFID_Status.RFID_Status; }
    inline uint8_t Get_RFID_Status_2() { return RFID_Status.RFID_Status_2; }
    // 飞镖相关
    inline uint8_t Get_Dart_Remaining_Time() { return Dart_Info.Dart_Remaining_Time; }
    inline uint16_t Get_Dart_Info() { return Dart_Info.Dart_Info; }
    inline Enum_Referee_Dart_Status Get_Dart_Launch_Status() { return static_cast<Enum_Referee_Dart_Status>(Dart_Cmd.Launch_Status); }
    inline uint16_t Get_Dart_Target_Change_Time() { return Dart_Cmd.Target_Change_Time; }
    inline uint16_t Get_Dart_Latest_Launch_Cmd_Time() { return Dart_Cmd.Latest_Launch_Cmd_Time; }
    // 哨兵专属
    inline Struct_Referee_Rx_Ground_Pos Get_Ground_Robot_Pos() { return Ground_Pos; }
    inline Struct_Referee_Rx_Sentry_Sync Get_Sentry_Sync_Info() { return Sentry_Sync; }
    // 雷达专属
    inline uint8_t Get_Radar_Sync_Info() { return Radar_Sync.Radar_Info; }
    // 交互相关
    inline Struct_Referee_Rx_Map_Cmd Get_Map_Cmd() { return Map_Cmd; }
    inline Struct_Referee_Rx_Radar_Map_Data Get_Radar_Map_Data() { return Radar_Map_Data; }
    inline Struct_Referee_Rx_Path_Data Get_Path_Data() { return Path_Data; }
    inline Struct_Referee_Rx_Custom_Info Get_Custom_Info() { return Custom_Info; }


    uint8_t CRC8_Calc(uint8_t *Message, uint32_t Length);
    uint16_t CRC16_Calc(uint8_t *Message, uint32_t Length);
    uint8_t CRC8_Verify(uint8_t *Message, uint32_t Length);
    uint8_t CRC16_Verify(uint8_t *Message, uint32_t Length);

    void Append_CRC8(uint8_t *Message, uint32_t Length);
    void Append_CRC16(uint8_t *Message, uint32_t Length);

private:

    // 裁判系统使用的串口
    std::shared_ptr<BSP_USART> usart;

    uint8_t Frame_Header;

    // 在线检测变量
    uint32_t Flag = 0;
    uint32_t Pre_Flag = 0;
    Enum_Referee_Status Referee_Status = Referee_Status_DISABLE;

    // 接收数据存储
    Struct_Referee_Rx_Game_Status Game_Status;
    Struct_Referee_Rx_Game_Result Game_Result;
    Struct_Referee_Rx_Robot_HP Robot_HP;
    Struct_Referee_Rx_Event_Data Event_Data;
    Struct_Referee_Rx_Warning Warning;
    Struct_Referee_Rx_Dart_Info Dart_Info;
    Struct_Referee_Rx_Robot_Status Robot_Status;
    Struct_Referee_Rx_Power_Heat Power_Heat;
    Struct_Referee_Rx_Robot_Position Robot_Position;
    Struct_Referee_Rx_Robot_Buff Robot_Buff;
    Struct_Referee_Rx_Damage Damage;
    Struct_Referee_Rx_Shoot Shoot;
    Struct_Referee_Rx_Ammo_Remain Ammo_Remain;
    Struct_Referee_Rx_RFID RFID_Status;
    Struct_Referee_Rx_Dart_Cmd Dart_Cmd;
    Struct_Referee_Rx_Ground_Pos Ground_Pos;
    Struct_Referee_Rx_Sentry_Sync Sentry_Sync;
    Struct_Referee_Rx_Radar_Sync Radar_Sync;
    Struct_Referee_Rx_Map_Cmd Map_Cmd;
    Struct_Referee_Rx_Radar_Map_Data Radar_Map_Data;
    Struct_Referee_Rx_Path_Data Path_Data;
    Struct_Referee_Rx_Custom_Info Custom_Info;


};



#endif
