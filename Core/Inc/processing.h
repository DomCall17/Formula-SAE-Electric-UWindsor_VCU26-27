#ifndef PROCESSING_H
#define PROCESSING_H

#include "stm32h7xx_hal_fdcan.h"
#include "can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define BATCH_TIMEOUT_MS pdMS_TO_TICKS(50)

typedef struct {
    FDCAN_HandleTypeDef *hfdcan; 
    QueueHandle_t queue;         
} can_task_params_t;

extern FDCAN_TxHeaderTypeDef msg;
extern FDCAN_TxHeaderTypeDef fault_msg;
extern FDCAN_TxHeaderTypeDef RTD_msg;

TaskHandle_t process_back_msg = NULL;
TaskHandle_t process_front_msg = NULL;
TaskHandle_t process_fault_msg = NULL;
TaskHandle_t process_sensor_msg = NULL;

void process_back_msg_task(void* pvParameters);
void process_front_msg_task(void* pvParameters);
void process_fault_msg_task(void* pvParameters);
void process_sensor_msg_task(void* pvParameters);

#endif
