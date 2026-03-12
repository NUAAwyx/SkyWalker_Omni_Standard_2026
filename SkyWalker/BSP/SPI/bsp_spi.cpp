#include "bsp_spi.h"

/**
 * @brief  SPI初始化
 *
 * @param hspi SPI句柄
 */
void BSP_SPI::Init(SPI_HandleTypeDef* hspi)
{
    _hspi = hspi;
    HAL_SPI_Init(_hspi);
}

/**
 * @brief  SPI发送数据
 *
 * @param txData 发送数据
 * @param num    数据长度
 */
void BSP_SPI::Transmit(uint8_t* txData,uint16_t num)
{
    HAL_SPI_Transmit(_hspi, txData, num, 1000);
}

/**
 * @brief  SPI发送和接收数据
 *
 * @param tx_data 发送数组
 * @param rx_data 接收数组
 * @param num     数据长度
 */
void BSP_SPI::Transmit_and_Receive(uint8_t* tx_data, uint8_t *rx_data,uint16_t num)
{
    HAL_SPI_TransmitReceive(_hspi, tx_data, rx_data, num, 1000);
}

/**
 * @brief  SPI片选拉高
 *
 * @param _gpio_port 片选端口
 * @param _gpio_pin  片选引脚
 */
void BSP_SPI::CS_High(GPIO_TypeDef* _gpio_port, uint16_t _gpio_pin)
{
    HAL_GPIO_WritePin(_gpio_port, _gpio_pin, GPIO_PIN_SET);
}

/**
 * @brief  SPI片选拉低
 *
 * @param _gpio_port 片选端口
 * @param _gpio_pin  片选引脚
 */
void BSP_SPI::CS_Low(GPIO_TypeDef* _gpio_port, uint16_t _gpio_pin)
{
    HAL_GPIO_WritePin(_gpio_port, _gpio_pin, GPIO_PIN_RESET);
}

