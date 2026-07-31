#ifndef PROCESSING_H
#define PROCESSING_H


#include "main.h"
#include "stm32h7xx_hal_fdcan.h"
#include "can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define BATCH_TIMEOUT_MS pdMS_TO_TICKS(50)

typedef struct {
    FDCAN_HandleTypeDef *hfdcan_write;
    QueueHandle_t queue;         
} can_task_params_t;


extern QueueHandle_t back_msg_queue;
extern QueueHandle_t fault_msg_queue;
extern QueueHandle_t front_msg_queue;
extern QueueHandle_t sensor_msg_queue;

extern FDCAN_TxHeaderTypeDef msg;
extern FDCAN_TxHeaderTypeDef fault_msg;
extern FDCAN_TxHeaderTypeDef RTD_msg;

extern TaskHandle_t process_back_msg;
extern TaskHandle_t process_front_msg;
extern TaskHandle_t process_fault_msg;
extern TaskHandle_t process_sensor_msg;

void process_back_msg_task(void* pvParameters);
void process_fault_msg_task(void* pvParameters);
void process_sensor_msg_task(void* pvParameters);

#endif
