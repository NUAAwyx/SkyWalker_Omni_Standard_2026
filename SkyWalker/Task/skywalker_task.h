#ifndef SKYWALKER_TASK_H
#define SKYWALKER_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

void Task_Init();
void StartIMUTask(void *argument);
void StartChassisTask(void *argument);
void StartGimbalTask(void *argument);

#ifdef __cplusplus
}
#endif


#endif