#include "skywalker_task.h"

#include "cmsis_os2.h"
#include "dvc_bmi088.h"
#include "dvc_motor_dji.h"
#include "dvc_motor_dm.h"

#include <vector>

DVC_BMI088 BMI088;

static std::vector<std::shared_ptr<DVC_Motor_DJI>> chassis_motors;
static std::shared_ptr<DVC_Motor_DM> yaw_motor;

void Task_Init()
{
    // 任务初始化代码

    // BSP层初始化部分

    // 开启CAN通信
    fdcan1->Begin();
    fdcan2->Begin();
    fdcan3->Begin();

    // Modules层初始化部分

    // 创建电机
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 1, M3508));
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 2, M3508));
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 3, M3508));
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 4, M3508));

    yaw_motor = std::make_shared<DVC_Motor_DM>(fdcan2, Motor_DM_MODE_MIT, 0x11, 0x01);

    // 以下部分用于测试
    chassis_motors[0]->Set_Data_to_send(2000);
    chassis_motors[1]->Set_Data_to_send(2000);
    chassis_motors[2]->Set_Data_to_send(2000);
    chassis_motors[3]->Set_Data_to_send(2000);

    yaw_motor->Set_MIT_K_P(0);
    yaw_motor->Set_MIT_K_D(0);
    yaw_motor->Set_Position_Designated(0);
    yaw_motor->Set_Velocity_Designated(0);
    yaw_motor->Set_Torque_Designated(10);

    BMI088.Init();


}

void StartIMUTask(void *argument)
{
    // IMU任务代码
    for(;;)
    {
        // 读取BMI088数据并处理
        BMI088.Read_Data();

        osDelay(10); // 延时10ms
    }
}

void StartChassisTask(void *argument)
{
    for(;;)
    {
        fdcan3->FDCAN_Transmit_Task();
        osDelay(10);
    }
}

void StartGimbalTask(void *argument)
{
    for (;;)
    {
        fdcan2->FDCAN_Transmit_Task();
        osDelay(10);
    }
}