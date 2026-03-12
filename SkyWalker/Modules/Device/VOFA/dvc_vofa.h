#ifndef DVC_VOFA_H
#define DVC_VOFA_H

#include "bsp_usart.h"

class DVC_VOFA
{
public:

    void Init();

private:

    uint8_t Rx_Data[10];
    uint8_t Tx_Data[10];

};



#endif