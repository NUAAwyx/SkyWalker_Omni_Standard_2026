#ifndef BMI088_H
#define BMI088_H

#include "bsp_spi.h"
#include "BMI088reg.h"
#include <memory>

#define BMI088_Read_Musk 0x80
#define BMI088_Send_Dummy_Byte 0x55

#define BMI088_MAX_DATA_LENGTH 10

#define BMI088_LONG_DELAY_TIME 80
#define BMI088_COM_WAIT_SENSOR_TIME 150

#define BMI088_TEMP_FACTOR 0.125f
#define BMI088_TEMP_OFFSET 23.0f

#define BMI088_ACCEL_RANGE_3G
#define BMI088_ACCEL_RANGE_6G
#define BMI088_ACCEL_RANGE_12G
#define BMI088_ACCEL_RANGE_24G

#define BMI088_GYRO_RANGE_2000
#define BMI088_GYRO_RANGE_1000
#define BMI088_GYRO_RANGE_500
#define BMI088_GYRO_RANGE_250
#define BMI088_GYRO_RANGE_125


#define BMI088_ACCEL_3G_SEN  0.0008974358974f
#define BMI088_ACCEL_6G_SEN  0.00179443359375f
#define BMI088_ACCEL_12G_SEN 0.0035888671875f
#define BMI088_ACCEL_24G_SEN 0.007177734375f


#define BMI088_GYRO_2000_SEN 0.00106526443603169529841533860381f
#define BMI088_GYRO_1000_SEN 0.00053263221801584764920766930190693f
#define BMI088_GYRO_500_SEN  0.00026631610900792382460383465095346f
#define BMI088_GYRO_250_SEN  0.00013315805450396191230191732547673f
#define BMI088_GYRO_125_SEN  0.000066579027251980956150958662738366f



class DVC_BMI088
{
public:

    DVC_BMI088(std::shared_ptr<BSP_SPI> spi_);
    void Init();
    void Accel_Init();
    void Gyro_Init();

    void Read_Data();

private:

    void Set_Reg(GPIO_TypeDef* _cs_port,uint16_t _cs_pin,uint16_t send_num);
    void Get_Reg(GPIO_TypeDef* _cs_port,uint16_t _cs_pin);
    void Set_Tx_Buffer(uint8_t reg_add, uint8_t data, uint16_t receive_num);

    void BMI088_delay_us(uint16_t us);
    void BMI088_delay_ms(uint16_t ms);

    std::shared_ptr<BSP_SPI> BMI088_SPI;

    int16_t bmi088_accel_x,bmi088_accel_y,bmi088_accel_z;
    int16_t bmi088_gyro_x,bmi088_gyro_y,bmi088_gyro_z;
    int16_t bmi088_temp;

    float BMI088_Accel_X, BMI088_Accel_Y, BMI088_Accel_Z;
    float BMI088_Gyro_X, BMI088_Gyro_Y, BMI088_Gyro_Z;
    float BMI088_Temp;

    uint8_t BMI088_Tx_Buffer[BMI088_MAX_DATA_LENGTH] = {0};
    uint8_t BMI088_Rx_Buffer[BMI088_MAX_DATA_LENGTH] = {0};
    uint8_t BMI088_Now_Need_Data_Num;


    float BMI088_ACCEL_SEN = BMI088_ACCEL_6G_SEN;
    float BMI088_GYRO_SEN = BMI088_GYRO_2000_SEN;
};


#endif
