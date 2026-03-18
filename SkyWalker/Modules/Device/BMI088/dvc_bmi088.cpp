#include "dvc_bmi088.h"

/**
 * @brief  BMI088构造函数
 *
 */
DVC_BMI088::DVC_BMI088(std::shared_ptr<BSP_SPI> spi_)
    :BMI088_SPI(spi_)
{
    Accel_Init();
    Gyro_Init();
}

/**
 * @brief  BMI088加速度计初始化
 *
 */
void DVC_BMI088::Accel_Init()
{
    // 读取加速度计芯片ID寄存器，验证通信是否正常
    do
    {
        Set_Tx_Buffer(BMI088_ACC_CHIP_ID | BMI088_Read_Musk,BMI088_Send_Dummy_Byte,2); // 加速度计会返回哑元
        Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_ACC_CHIP_ID_VALUE);

    // 软复位加速度计，手册5.3.22
    Set_Tx_Buffer(BMI088_ACC_SOFTRESET,BMI088_ACC_SOFTRESET_VALUE,2);
    Set_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, 2);
    BMI088_delay_ms(BMI088_LONG_DELAY_TIME);

    // 复位完成后再次验证通信
    do
    {
        Set_Tx_Buffer(BMI088_ACC_CHIP_ID | BMI088_Read_Musk,BMI088_Send_Dummy_Byte,2);
        Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_ACC_CHIP_ID_VALUE);

    // 上电加速度计，手册5.3.21
    do
    {
        Set_Tx_Buffer(BMI088_ACC_PWR_CTRL, BMI088_ACC_ENABLE_ACC_ON, 2);
        Set_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_ACC_PWR_CTRL | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 2);
        Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_ACC_ENABLE_ACC_ON);

    // 加速度计切换至活动模式，手册5.3.20
    do
    {
        Set_Tx_Buffer(BMI088_ACC_PWR_CONF, BMI088_ACC_PWR_ACTIVE_MODE, 2);
        Set_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_ACC_PWR_CONF | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 2);
        Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_ACC_PWR_ACTIVE_MODE);

    // 设置加速度计无低通滤波器，反馈频率1600Hz，手册5.3.10
    do
    {
        Set_Tx_Buffer(BMI088_ACC_CONF, (BMI088_ACC_NORMAL | BMI088_ACC_1600_HZ | BMI088_ACC_CONF_MUST_Set), 2); // 为了修正模式寄存器的值，必须设置BMI088_ACC_CONF_MUST_Set位
        Set_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_ACC_CONF | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 2);
        Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != (BMI088_ACC_NORMAL | BMI088_ACC_1600_HZ | BMI088_ACC_CONF_MUST_Set));

    // 设置加速度计量程，手册5.3.11
    do
    {
        Set_Tx_Buffer(BMI088_ACC_RANGE, BMI088_ACC_RANGE_6G, 2);
        Set_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_ACC_RANGE | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 2);
        Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_ACC_RANGE_6G);

    // 设置中INT1引脚为推挽模式低电平有效，手册5.3.16
    do
    {
        Set_Tx_Buffer(BMI088_INT1_IO_CTRL, (BMI088_ACC_INT1_IO_ENABLE | BMI088_ACC_INT1_GPIO_PP | BMI088_ACC_INT1_GPIO_LOW), 2);
        Set_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_INT1_IO_CTRL | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 2);
        Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != (BMI088_ACC_INT1_IO_ENABLE | BMI088_ACC_INT1_GPIO_PP | BMI088_ACC_INT1_GPIO_LOW));

    // 配置为如果数据准备好中断映射到INT1，手册5.3.18，因为设置了数据准备好就中断，故不需要设置fifo，硬件上只将INT1分配给了加速度计
    do
    {
        Set_Tx_Buffer(BMI088_INT_MAP_DATA, BMI088_ACC_INT1_DRDY_INTERRUPT, 2);
        Set_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_INT_MAP_DATA | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 2);
        Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_ACC_INT1_DRDY_INTERRUPT);
}

/**
 * @brief  BMI088陀螺仪初始化
 *
 */
void DVC_BMI088::Gyro_Init()
{
    // 读取陀螺仪芯片ID寄存器，验证通信是否正常
    do
    {
        Set_Tx_Buffer(BMI088_GYRO_CHIP_ID | BMI088_Read_Musk,BMI088_Send_Dummy_Byte,1); // 陀螺仪不会返回哑元
        Get_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_GYRO_CHIP_ID_VALUE);

    // 软复位陀螺仪，手册5.5.8
    Set_Tx_Buffer(BMI088_GYRO_SOFTRESET,BMI088_GYRO_SOFTRESET_VALUE,1);
    Set_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin, 2);
    BMI088_delay_ms(BMI088_LONG_DELAY_TIME);

    // 复位完成后再次验证通信
    do
    {
        Set_Tx_Buffer(BMI088_GYRO_CHIP_ID | BMI088_Read_Musk,BMI088_Send_Dummy_Byte,1);
        Get_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_GYRO_CHIP_ID_VALUE);

    // 设置陀螺仪量程2000°/s，手册5.5.5
    do
    {
        Set_Tx_Buffer(BMI088_GYRO_RANGE, BMI088_GYRO_2000, 1);
        Set_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_GYRO_RANGE | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 1);
        Get_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_GYRO_2000);

     // 设置陀螺仪反馈频率为2000Hz, 带宽为532Hz，手册5.5.6
    do
    {
        Set_Tx_Buffer(BMI088_GYRO_BANDWIDTH, (BMI088_GYRO_2000_532_HZ | BMI088_GYRO_BANDWIDTH_MUST_Set), 1);
        Set_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_GYRO_BANDWIDTH | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 1);
        Get_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }
    while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != (BMI088_GYRO_2000_532_HZ | BMI088_GYRO_BANDWIDTH_MUST_Set));

    // 设置中断输出模式，手册5.5.9
    do
    {
        Set_Tx_Buffer(BMI088_GYRO_INT_CTRL, BMI088_DRDY_ON, 1);
        Set_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_GYRO_INT_CTRL | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 1);
        Get_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_DRDY_ON);

    // 设置INT3引脚为推挽输出，低电平有效，手册5.5.10
    do
    {
        Set_Tx_Buffer(BMI088_GYRO_INT3_INT4_IO_CONF, (BMI088_GYRO_INT3_GPIO_PP | BMI088_GYRO_INT3_GPIO_LOW), 1);
        Set_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_GYRO_INT3_INT4_IO_CONF | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 1);
        Get_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != (BMI088_GYRO_INT3_GPIO_PP | BMI088_GYRO_INT3_GPIO_LOW));

    // 因为设置了数据准备好就中断，手册5.5.11，故不需要设置fifo，硬件上只将INT3分配给了陀螺仪，所以INT4不需要设置
    do
    {
        Set_Tx_Buffer(BMI088_GYRO_INT3_INT4_IO_MAP, BMI088_GYRO_DRDY_IO_INT3, 1);
        Set_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin, 2);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        Set_Tx_Buffer(BMI088_GYRO_INT3_INT4_IO_MAP | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 1);
        Get_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin);
        BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    }while (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] != BMI088_GYRO_DRDY_IO_INT3);
}

/**
 * @brief  BMI088读取加速度计，陀螺仪和温度数据
 *
 */
void DVC_BMI088::Read_Data()
{
    // 读取加速度计数据，手册5.3.4
    Set_Tx_Buffer(BMI088_ACCEL_XOUT_L | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 7);
    Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    bmi088_accel_x = (int16_t)((BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 5] << 8) | BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 6]);
    bmi088_accel_y = (int16_t)((BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 3] << 8) | BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 4]);
    bmi088_accel_z = (int16_t)((BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] << 8) | BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 2]);

    BMI088_Accel_X =  bmi088_accel_x * BMI088_ACCEL_SEN;
    BMI088_Accel_Y =  bmi088_accel_y * BMI088_ACCEL_SEN;
    BMI088_Accel_Z =  bmi088_accel_z * BMI088_ACCEL_SEN;

    // 读取陀螺仪数据，手册5.5.2
    Set_Tx_Buffer(BMI088_GYRO_X_L | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 6);
    Get_Reg(GYRO_CS_GPIO_Port, GYRO_CS_Pin);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    bmi088_gyro_x = (int16_t)((BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 5] << 8) | BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 6]);
    bmi088_gyro_y = (int16_t)((BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 3] << 8) | BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 4]);
    bmi088_gyro_z = (int16_t)((BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] << 8) | BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 2]);

    BMI088_Gyro_X =  bmi088_gyro_x * BMI088_GYRO_SEN;
    BMI088_Gyro_Y =  bmi088_gyro_y * BMI088_GYRO_SEN;
    BMI088_Gyro_Z =  bmi088_gyro_z * BMI088_GYRO_SEN;

    // 读取温度数据，手册5.3.7
    Set_Tx_Buffer(BMI088_TEMP_M | BMI088_Read_Musk, BMI088_Send_Dummy_Byte, 3);
    Get_Reg(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin);
    BMI088_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    bmi088_temp = (int16_t)((BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 2] << 3) | (BMI088_Rx_Buffer[BMI088_Now_Need_Data_Num - 1] >> 5));

    if (bmi088_temp > 1023)
    {
        bmi088_temp -= 2048;
    }

    BMI088_Temp = bmi088_temp * BMI088_TEMP_FACTOR + BMI088_TEMP_OFFSET;
}

/**
 * @brief  BMI088写入寄存器数据，一般为单寄存器写入
 *
 * @param _cs_port 片选端口
 * @param _cs_pin  片选引脚
 * @param send_num      要写入的数据长度
 */
void DVC_BMI088::Set_Reg(GPIO_TypeDef* _cs_port, uint16_t _cs_pin, uint16_t send_num)
{
    BMI088_SPI->CS_Low(_cs_port, _cs_pin);
    BMI088_SPI->Transmit(BMI088_Tx_Buffer, send_num);
    BMI088_SPI->CS_High(_cs_port, _cs_pin);
}

/**
 * @brief  BMI088读取寄存器数据，可以连读多个寄存器
 *
 * @param _cs_port 片选端口
 * @param _cs_pin  片选引脚
 */
void DVC_BMI088::Get_Reg(GPIO_TypeDef* _cs_port,uint16_t _cs_pin)
{
    BMI088_SPI->CS_Low(_cs_port, _cs_pin);
    BMI088_SPI->Transmit_and_Receive(BMI088_Tx_Buffer, BMI088_Rx_Buffer, BMI088_Now_Need_Data_Num);
    BMI088_SPI->CS_High(_cs_port, _cs_pin);
}

/**
 * @brief 设置发送缓冲区数据
 *
 * @param reg_add  需要设置的寄存器地址
 * @param data     要发送的数值
 * @param receive_num      要接收的数据长度
 */
void DVC_BMI088::Set_Tx_Buffer(uint8_t reg_add, uint8_t data, uint16_t receive_num)
{
    // 将寄存器地址和数据放入发送缓冲区前两位
    BMI088_Tx_Buffer[0] = reg_add;
    BMI088_Tx_Buffer[1] = data;

    // 将发送缓冲区的剩余部分填充为0x55补齐长度
    for (uint16_t i = 2; i < receive_num; i++)
    {
        BMI088_Tx_Buffer[i] = BMI088_Send_Dummy_Byte;
    }

    // 更新当前需要发送的数据长度,+1是因为主机发送的时候从机返回的部分也需要占用
    BMI088_Now_Need_Data_Num = receive_num + 1;
}

/**
 * @brief BMI088微秒级延时函数
 *
 * @param us 延时微秒数
 */
void DVC_BMI088::BMI088_delay_us(uint16_t us)
{
    uint32_t ticks = 0;
    uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    uint32_t reload = 0;
    reload = SysTick->LOAD;
    ticks = us * 480;
    told = SysTick->VAL;
    while (true)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

/**
 * @brief BMI088毫秒级延时函数
 *
 * @param ms 延时毫秒数
 */
void DVC_BMI088::BMI088_delay_ms(uint16_t ms)
{
    while(ms--)
    {
        BMI088_delay_us(1000);
    }
}

/**
 * @brief 获取X方向加速度
 *
 */
float DVC_BMI088::Get_Accel_X()
{
    return BMI088_Accel_X;
}

/**
 * @brief 获取Y方向加速度
 *
 */
float DVC_BMI088::Get_Accel_Y()
{
    return BMI088_Accel_Y;
}

/**
 * @brief 获取Z方向加速度
 *
 */
float DVC_BMI088::Get_Accel_Z()
{
    return BMI088_Accel_Z;
}

/**
 * @brief 获取X方向角速度
 *
 */
float DVC_BMI088::Get_Gyro_X()
{
    return BMI088_Gyro_X;
}

/**
 * @brief 获取Y方向角速度
 *
 */
float DVC_BMI088::Get_Gyro_Y()
{
    return BMI088_Gyro_Y;
}

/**
 * @brief 获取Z方向角速度
 *
 */
float DVC_BMI088::Get_Gyro_Z()
{
    return BMI088_Gyro_Z;
}