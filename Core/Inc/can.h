#ifndef CAN_H
#define CAN_H

#include "stm32h7xx_hal_fdcan.h"
#include "FreeRTOS.h"
#include "queue.h"

#define MSG_ID (uint16_t)0x0
#define FAULT_ID (uint16_t)0x01
#define RTD_ID (uint16_t)0x02
#define FIFO0 FDCAN_RX_FIFO0
#define FIFO1 FDCAN_RX_FIFO1
#define BACK_MSG_QUEUE_LEN (uint8_t)16
#define FAULT_MSG_QUEUE_LEN (uint8_t)16
#define FRONT_MSG_QUEUE_LEN (uint8_t)16
#define QUEUE_THRESHOLD (uint8_t)12 

extern FDCAN_HandleTypeDef can1;
extern FDCAN_HandleTypeDef can2;

FDCAN_TxHeaderTypeDef msg;
FDCAN_TxHeaderTypeDef fault_msg;
FDCAN_TxHeaderTypeDef RTD_msg;


typedef struct{
    uint16_t id;
    uint8_t data[8];
    uint32_t timestamp;
}rx_msg;

QueueHandle_t back_msg_queue;
QueueHandle_t fault_msg_queue;
QueueHandle_t front_msg_queue;
QueueHandle_t sensor_msg_queue;

extern TaskHandle_t process_back_msg;
extern TaskHandle_t process_front_msg;
extern TaskHandle_t process_fault_msg;
extern TaskHandle_t process_sensor_msg;

static HAL_StatusTypeDef config_canbus(FDCAN_HandleTypeDef *header, FDCAN_GlobalTypeDef *instance);
static void config_tx_msg(FDCAN_TxHeaderTypeDef *header, uint16_t id);
void can_init(void);
void send_to_queue(FDCAN_HandleTypeDef *hfdcan, uint32_t FIFO, FDCAN_RxHeaderTypeDef *header, rx_msg* msg_header, QueueHandle_t queue, TaskHandle_t task);

#endif