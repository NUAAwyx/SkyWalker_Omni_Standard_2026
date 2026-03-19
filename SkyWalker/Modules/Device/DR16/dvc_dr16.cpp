#include "dvc_dr16.h"

std::shared_ptr<DVC_DR16> DR16 = std::make_shared<DVC_DR16>(uart5);

/**
 * @brief 遥控器构造函数
 *
 */
DVC_DR16::DVC_DR16(std::shared_ptr<BSP_USART> uart_)
    : uart(uart_)
{
    uart->Register_ReceiveCallback([this](const Struct_UART_Receive_Management& Receive_Management)
    {
        this->Handle_Receive_Data(Receive_Management);
    });
}

/**
 * @brief 遥控器数据处理
 *
 */
void DVC_DR16::Handle_Receive_Data(const Struct_UART_Receive_Management& Receive_Management)
{

    Right_X = (Receive_Management.rx_buffer[0] | (Receive_Management.rx_buffer[1] << 8)) & 0x07ff;         //!< Channel 0
    Right_Y = ((Receive_Management.rx_buffer[1] >> 3) | (Receive_Management.rx_buffer[2] << 5)) & 0x07ff;  //!< Channel 1
    Left_Y = ((Receive_Management.rx_buffer[2] >> 6) | (Receive_Management.rx_buffer[3] << 2) |            //!< Channel 2
                          (Receive_Management.rx_buffer[4] << 10)) &0x07ff;
    Left_X = ((Receive_Management.rx_buffer[4] >> 1) | (Receive_Management.rx_buffer[5] << 7)) & 0x07ff;   //!< Channel 3
    switch_R = ((Receive_Management.rx_buffer[5] >> 4) & 0x0003);                                          //!< Switch left
    switch_L = ((Receive_Management.rx_buffer[5] >> 4) & 0x000C) >> 2;                                     //!< Switch right
    mouse_x = Receive_Management.rx_buffer[6] | (Receive_Management.rx_buffer[7] << 8);                    //!< Mouse X axis
    mouse_y = Receive_Management.rx_buffer[8] | (Receive_Management.rx_buffer[9] << 8);                    //!< Mouse Y axis
    mouse_z = Receive_Management.rx_buffer[10] | (Receive_Management.rx_buffer[11] << 8);                  //!< Mouse Z axis
    mouse_press_l = Receive_Management.rx_buffer[12];                                                      //!< Mouse Left Is Press ?
    mouse_press_r = Receive_Management.rx_buffer[13];                                                      //!< Mouse Right Is Press ?
    key = Receive_Management.rx_buffer[14] | (Receive_Management.rx_buffer[15] << 8);                      //!< KeyBoard value
    Left_Front_Wheel = Receive_Management.rx_buffer[16] | (Receive_Management.rx_buffer[17] << 8);

    Right_X -= RC_CH_VALUE_OFFSET;
    Right_Y -= RC_CH_VALUE_OFFSET;
    Left_X -= RC_CH_VALUE_OFFSET;
    Left_Y -= RC_CH_VALUE_OFFSET;
    Left_Front_Wheel -= RC_CH_VALUE_OFFSET;

    //Left_Y *= -1;
    Right_X *= -1;
    //Left_Front_Wheel *= -1;

    w = key & KEY_PRESSED_OFFSET_W;
    s = key & KEY_PRESSED_OFFSET_S;
    a = key & KEY_PRESSED_OFFSET_A;
    d = key & KEY_PRESSED_OFFSET_D;
    shift = key & KEY_PRESSED_OFFSET_SHIFT;
    ctrl = key & KEY_PRESSED_OFFSET_CTRL;
    q = key & KEY_PRESSED_OFFSET_Q;
    e = key & KEY_PRESSED_OFFSET_E;
    r = key & KEY_PRESSED_OFFSET_R;
    f = key & KEY_PRESSED_OFFSET_F;
    g = key & KEY_PRESSED_OFFSET_G;
    z = key & KEY_PRESSED_OFFSET_Z;
    x = key & KEY_PRESSED_OFFSET_X;
    c = key & KEY_PRESSED_OFFSET_C;
    v = key & KEY_PRESSED_OFFSET_V;
    b = key & KEY_PRESSED_OFFSET_B;
}


/**
 * @brief 获取右拨杆X方向值
 *
 */
int16_t DVC_DR16::Get_Right_X()
{
    return Right_X;
}

/**
 * @brief 获取右拨杆Y方向值
 *
 */
int16_t DVC_DR16::Get_Right_Y()
{
    return Right_Y;
}

/**
 * @brief 获取左拨杆X方向值
 *
 */
int16_t DVC_DR16::Get_Left_X()
{
    return Left_X;
}

/**
 * @brief 获取左拨杆Y方向值
 *
 */
int16_t DVC_DR16::Get_Left_Y()
{
    return Left_Y;
}

/**

@brief 获取前轮值

*/
int16_t DVC_DR16::Get_Front_Wheel()
{
    return Left_Front_Wheel;
}

/**

@brief 获取左拨动开关状态

*/
uint8_t DVC_DR16::Get_Switch_L()
{
    return switch_L;
}

/**

@brief 获取右拨动开关状态

*/
uint8_t DVC_DR16::Get_Switch_R()
{
    return switch_R;
}

/**

@brief 获取鼠标X轴值

*/
int16_t DVC_DR16::Get_Mouse_X()
{
    return mouse_x;
}

/**

@brief 获取鼠标Y轴值

*/
int16_t DVC_DR16::Get_Mouse_Y()
{
    return mouse_y;
}

/**

@brief 获取鼠标Z轴值（滚轮）

*/
int16_t DVC_DR16::Get_Mouse_Z()
{
    return mouse_z;
}

/**

@brief 获取鼠标左键按下状态

*/
uint8_t DVC_DR16::Get_Mouse_Press_L()
{
    return mouse_press_l;
}

/**

@brief 获取鼠标右键按下状态

*/
uint8_t DVC_DR16::Get_Mouse_Press_R()
{
    return mouse_press_r;
}

/**

@brief 获取鼠标中键按下状态

*/
uint8_t DVC_DR16::Get_Mouse_Press_M()
{
    return mouse_press_m;
}

/**

@brief 获取W键状态

*/
uint8_t DVC_DR16::Get_Key_W()
{
    return w;
}

/**

@brief 获取S键状态

*/
uint8_t DVC_DR16::Get_Key_S()
{
    return s;
}

/**

@brief 获取A键状态

*/
uint8_t DVC_DR16::Get_Key_A()
{
    return a;
}

/**

@brief 获取D键状态

*/
uint8_t DVC_DR16::Get_Key_D()
{
    return d;
}

/**

@brief 获取Shift键状态

*/
uint8_t DVC_DR16::Get_Key_Shift()
{
    return shift;
}

/**

@brief 获取Ctrl键状态

*/
uint8_t DVC_DR16::Get_Key_Ctrl()
{
    return ctrl;
}

/**

@brief 获取Q键状态

*/
uint8_t DVC_DR16::Get_Key_Q()
{
    return q;
}

/**

@brief 获取E键状态

*/
uint8_t DVC_DR16::Get_Key_E()
{
    return e;
}

/**

@brief 获取R键状态

*/
uint8_t DVC_DR16::Get_Key_R()
{
    return r;
}

/**

@brief 获取F键状态

*/
uint8_t DVC_DR16::Get_Key_F()
{
    return f;
}

/**

@brief 获取G键状态

*/
uint8_t DVC_DR16::Get_Key_G()
{
    return g;
}

/**

@brief 获取Z键状态

*/
uint8_t DVC_DR16::Get_Key_Z()
{
    return z;
}

/**

@brief 获取X键状态

*/
uint8_t DVC_DR16::Get_Key_X()
{
    return x;
}

/**

@brief 获取C键状态

*/
uint8_t DVC_DR16::Get_Key_C()
{
    return c;
}

/**

@brief 获取V键状态

*/
uint8_t DVC_DR16::Get_Key_V()
{
    return v;
}

/**

@brief 获取B键状态

*/
uint8_t DVC_DR16::Get_Key_B()
{
    return b;
}
