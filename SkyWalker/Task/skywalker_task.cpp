#include "skywalker_task.h"

#include "cmsis_os2.h"
#include "dvc_bmi088.h"
#include "dvc_motor_dm.h"

DVC_BMI088 BMI088;
DVC_MOTOR_DM motor_dm;

void Task_Init()
{
    // 任务初始化代码

    BMI088.Init();
    //motor_dm.Init();


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

}