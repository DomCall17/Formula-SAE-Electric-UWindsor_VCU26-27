#ifndef CAN_H
#define CAN_H

#include "main.h"
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
#define SENSOR_MSG_QUEUE_LEN (uint8_t)16
#define QUEUE_THRESHOLD (uint8_t)12 


extern FDCAN_HandleTypeDef can1;
extern FDCAN_HandleTypeDef can2;

extern FDCAN_FilterTypeDef sFilterConfig;

extern FDCAN_TxHeaderTypeDef msg;
extern FDCAN_TxHeaderTypeDef fault_msg;
extern FDCAN_TxHeaderTypeDef RTD_msg;


typedef struct{
    uint16_t id;
    uint8_t data[8];
    uint32_t timestamp;
}rx_msg;

extern QueueHandle_t back_msg_queue;
extern QueueHandle_t fault_msg_queue;
extern QueueHandle_t front_msg_queue;
extern QueueHandle_t sensor_msg_queue;


extern TaskHandle_t process_back_msg;
extern TaskHandle_t process_front_msg;
extern TaskHandle_t process_fault_msg;
extern TaskHandle_t process_sensor_msg;

HAL_StatusTypeDef config_canbus(FDCAN_HandleTypeDef *header, FDCAN_GlobalTypeDef *instance, uint16_t offset, uint8_t rxfifo0_elem_num, uint8_t rxfifo1_elem_num);
void config_tx_msg(FDCAN_TxHeaderTypeDef *header, uint16_t id);
void can_init(void);
void config_can_filter(uint8_t index, FDCAN_HandleTypeDef *hfdcan,FDCAN_FilterTypeDef *filter_header, uint32_t fifo, uint32_t id1, uint32_t id2);
void send_to_queue(FDCAN_HandleTypeDef *hfdcan, uint32_t FIFO, FDCAN_RxHeaderTypeDef *header, rx_msg* msg_header, QueueHandle_t queue, TaskHandle_t task);

#endif