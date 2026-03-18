#ifndef BSP_SPI_H
#define BSP_SPI_H

#include <memory>

#include "spi.h"

class BSP_SPI
{
public:

    BSP_SPI(SPI_HandleTypeDef* hspi_);
    void Transmit(uint8_t* txData,uint16_t num);
    void Transmit_and_Receive(uint8_t* tx_data, uint8_t *rx_data,uint16_t num);
    void CS_High(GPIO_TypeDef* _gpio_port, uint16_t _gpio_pin);
    void CS_Low(GPIO_TypeDef* _gpio_port, uint16_t _gpio_pin);

private:

    SPI_HandleTypeDef* hspi;
};

extern std::shared_ptr<BSP_SPI> spi2;   // 陀螺仪SPI

#endif