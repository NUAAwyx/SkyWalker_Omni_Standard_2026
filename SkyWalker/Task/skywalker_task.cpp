#include "skywalker_task.h"

#include "cmsis_os2.h"
#include "dvc_bmi088.h"
#include "dvc_motor_dji.h"
#include "dvc_motor_dm.h"
#include "dvc_vofa.h"

#include <vector>


static std::vector<std::shared_ptr<DVC_Motor_DJI>> chassis_motors;
static std::shared_ptr<DVC_Motor_DM> yaw_motor;
static std::shared_ptr<DVC_VOFA> VOFA;
static std::shared_ptr<DVC_BMI088> BMI088;

uint8_t vofa_send_data[4];

void Task_Init()
{
    // 开启CAN通信
    fdcan1->Begin();
    fdcan2->Begin();
    fdcan3->Begin();

    // 创建陀螺仪
    BMI088 = std::make_shared<DVC_BMI088>(spi2);

    // 创建电机
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 1, M3508, DJI_Velocity));
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 2, M3508, DJI_Velocity));
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 3, M3508, DJI_Velocity));
    chassis_motors.push_back(std::make_shared<DVC_Motor_DJI>(fdcan3, 4, M3508, DJI_Velocity));

    yaw_motor = std::make_shared<DVC_Motor_DM>(fdcan2, Motor_DM_MODE_MIT, 0x11, 0x01, 3.141593, 45, 18, DM_Position);

    // 创建VOFA，以供电机调参
    VOFA = std::make_shared<DVC_VOFA>(usart10,JustFloat);
    // 以下部分用于测试

    yaw_motor->Set_MIT_K_P(0);
    yaw_motor->Set_MIT_K_D(0);
    yaw_motor->Set_Position_Designated(0);
    yaw_motor->Set_Velocity_Designated(0);
    yaw_motor->Set_Torque_Designated(10);


}

void StartAimBoosterTask(void *argument)
{
    // IMU任务代码
    for(;;)
    {
        osDelay(10); // 延时10ms
    }
}

void StartChassisTask(void *argument)
{
    for(;;)
    {
        // 读取BMI088数据并处理，获取底盘角速度信息用于小陀螺
        BMI088->Read_Data();

        // 电机调参测试用代码
        switch (VOFA->Get_Cmd_Type())
        {
        case OP:
            chassis_motors[0]->Set_Velocity_PID_KP(VOFA->Get_Cmd_Value());
            break;
        case OI:
            chassis_motors[0]->Set_Velocity_PID_KI(VOFA->Get_Cmd_Value());
            break;
        case OD:
            chassis_motors[0]->Set_Velocity_PID_KD(VOFA->Get_Cmd_Value());
            break;
        case AP:
            chassis_motors[0]->Set_Position_PID_KP(VOFA->Get_Cmd_Value());
            break;
        case AI:
            chassis_motors[0]->Set_Position_PID_KI(VOFA->Get_Cmd_Value());
            break;
        case AD:
            chassis_motors[0]->Set_Position_PID_KD(VOFA->Get_Cmd_Value());
            break;
        }
        vofa_send_data[0] = chassis_motors[0]->Get_Target_Omega();
        vofa_send_data[1] = chassis_motors[0]->Get_Now_Omega();
        chassis_motors[0]->Motor_Control();
        // chassis_motors[1]->Motor_Control();
        // chassis_motors[2]->Motor_Control();
        // chassis_motors[3]->Motor_Control();

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