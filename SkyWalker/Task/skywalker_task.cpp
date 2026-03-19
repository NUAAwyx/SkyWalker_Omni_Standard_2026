#include "skywalker_task.h"

#include "app_aimbooster.h"
#include "app_chassis.h"
#include "app_gimbal.h"

#include "cmsis_os2.h"

#include "dvc_vofa.h"


//static std::shared_ptr<DVC_VOFA> VOFA;

uint8_t vofa_send_data[4];

void Task_Init()
{
    // 开启CAN通信
    fdcan1->Begin();
    fdcan2->Begin();
    fdcan3->Begin();

    Chassis->Initialize_Chassis();

    // 创建VOFA，以供电机调参
    //VOFA = std::make_shared<DVC_VOFA>(usart10,JustFloat);
    // 以下部分用于测试


}

void StartAimBoosterTask(void *argument)
{
    // IMU任务代码
    for(;;)
    {
        AimBooster->AimBooster_Control();
        osDelay(1); // 延时10ms
    }
}

void StartChassisTask(void *argument)
{
    for(;;)
    {
        // 读取BMI088数据并处理，获取底盘角速度信息用于小陀螺
        //BMI088->Read_Data();

        // 电机调参测试用代码
        // switch (VOFA->Get_Cmd_Type())
        // {
        // case OP:
        //     chassis_motors[0]->Set_Velocity_PID_KP(VOFA->Get_Cmd_Value());
        //     break;
        // case OI:
        //     chassis_motors[0]->Set_Velocity_PID_KI(VOFA->Get_Cmd_Value());
        //     break;
        // case OD:
        //     chassis_motors[0]->Set_Velocity_PID_KD(VOFA->Get_Cmd_Value());
        //     break;
        // case AP:
        //     chassis_motors[0]->Set_Position_PID_KP(VOFA->Get_Cmd_Value());
        //     break;
        // case AI:
        //     chassis_motors[0]->Set_Position_PID_KI(VOFA->Get_Cmd_Value());
        //     break;
        // case AD:
        //     chassis_motors[0]->Set_Position_PID_KD(VOFA->Get_Cmd_Value());
        //     break;
        // }
        // vofa_send_data[0] = chassis_motors[0]->Get_Target_Omega();
        // vofa_send_data[1] = chassis_motors[0]->Get_Now_Omega();

        // Chassis->Chassis_Control();
        osDelay(1);
    }
}

void StartGimbalTask(void *argument)
{
    for (;;)
    {
        Gimbal->Gimbal_Control();
        osDelay(10);
    }
}

void StartFDCANTransmitTask(void *argument)
{
    for (;;)
    {
        fdcan1->FDCAN_Transmit_Task();
        fdcan2->FDCAN_Transmit_Task();
        fdcan3->FDCAN_Transmit_Task();
        osDelay(1);
    }
}