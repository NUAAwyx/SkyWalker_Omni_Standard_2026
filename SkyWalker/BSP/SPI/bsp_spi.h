#ifndef BSP_SPI_H
#define BSP_SPI_H

#include "spi.h"

class BSP_SPI
{
public:

    void Init(SPI_HandleTypeDef* hspi);
    void Transmit(uint8_t* txData,uint16_t num);
    void Transmit_and_Receive(uint8_t* tx_data, uint8_t *rx_data,uint16_t num);
    void CS_High(GPIO_TypeDef* _gpio_port, uint16_t _gpio_pin);
    void CS_Low(GPIO_TypeDef* _gpio_port, uint16_t _gpio_pin);

private:

    SPI_HandleTypeDef* _hspi;
};

#endif