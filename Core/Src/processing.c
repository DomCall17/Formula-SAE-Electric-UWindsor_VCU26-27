#include "processing.h"
#include "can.h"

void process_msg_task(void* pvParameters)
{
    can_task_params_t *params = (can_task_params_t*) pvParameters;
    FDCAN_HandleTypeDef *hfdcan = params->hfdcan; 
    QueueHandle_t queue = params->queue;
    rx_msg msg_header;
    for(;;)
    {
        uint32_t notify_count = ulTaskNotifyTake(pdTRUE, BATCH_TIMEOUT_MS);

        while(xQueueReceive(queue, &msg_header, 0) == pdTRUE)
        {
            while(HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) == 0)
            {
                vTaskDelay(pdMS_TO_TICKS(1));
            }

            HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &msg, msg_header.data);
        }
    }
}

void process_fault_msg_task(void* pvParameters)
{
    for(;;)
    {
        
    }
}

void process_front_msg_task(void* pvParameters)
{
    for(;;)
    {

    }
}




