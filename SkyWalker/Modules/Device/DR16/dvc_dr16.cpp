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