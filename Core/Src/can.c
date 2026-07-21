#include "can.h"
#include "stm32h7xx_hal_fdcan.h"

FDCAN_HandleTypeDef can1 = {0};
FDCAN_HandleTypeDef can2 = {0};

FDCAN_TxHeaderTypeDef msg = {0}; 
FDCAN_TxHeaderTypeDef fault_msg = {0};
FDCAN_TxHeaderTypeDef RTD_msg = {0};

QueueHandle_t back_msg_queue = NULL;
QueueHandle_t fault_msg_queue = NULL;
QueueHandle_t front_msg_queue = NULL;
QueueHandle_t sensor_msg_queue = NULL;

static HAL_StatusTypeDef config_canbus(FDCAN_HandleTypeDef *header, FDCAN_GlobalTypeDef *instance)
{
  header->Instance = instance;
  header->Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  header->Init.Mode = FDCAN_MODE_NORMAL;
  header->Init.AutoRetransmission = ENABLE;
  header->Init.TransmitPause = DISABLE;
  header->Init.ProtocolException = DISABLE;
  header->Init.NominalPrescaler = 6;
  header->Init.NominalSyncJumpWidth = 13;
  header->Init.NominalTimeSeg1 = 12;
  header->Init.NominalTimeSeg2 = 3;
  header->Init.DataPrescaler = 1;
  header->Init.DataSyncJumpWidth = 1;
  header->Init.DataTimeSeg1 = 1;
  header->Init.DataTimeSeg2 = 1;
  header->Init.MessageRAMOffset = 1920;
  header->Init.StdFiltersNbr = 1;
  header->Init.ExtFiltersNbr = 0;
  header->Init.RxFifo0ElmtsNbr = 8;
  header->Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  header->Init.RxFifo1ElmtsNbr = 8;
  header->Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  header->Init.RxBuffersNbr = 0;
  header->Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  header->Init.TxEventsNbr = 0;
  header->Init.TxBuffersNbr = 0;
  header->Init.TxFifoQueueElmtsNbr = 8;
  header->Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  header->Init.TxElmtSize = FDCAN_DATA_BYTES_8;

  return HAL_FDCAN_Init(header);
}

static void config_tx_msg(FDCAN_TxHeaderTypeDef *header, uint16_t id)
{
    header->Identifier = id;
	header->IdType = FDCAN_STANDARD_ID;
	header->TxFrameType = FDCAN_DATA_FRAME;
	header->DataLength = FDCAN_DLC_BYTES_8;
	header->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	header->BitRateSwitch = FDCAN_BRS_OFF;
	header->FDFormat = FDCAN_CLASSIC_CAN;
	header->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	header->MessageMarker = 0;
}

void CAN_Init(void)
{
    config_canbus(&can1, FDCAN1);
    config_canbus(&can2, FDCAN2);
    HAL_FDCAN_Start(&can1);
    HAL_FDCAN_Start(&can2);

    config_tx_msg(&msg, MSG_ID);
    config_tx_msg(&fault_msg, FAULT_ID);
    config_tx_msg(&RTD_msg, RTD_ID);

    back_msg_queue = xQueueCreate(FRONT_MSG_QUEUE_LEN, sizeof(rx_msg));
    fault_msg_queue = xQueueCreate(FAULT_MSG_QUEUE_LEN, sizeof(rx_msg));
    front_msg_queue = xQueueCreate(BACK_MSG_QUEUE_LEN, sizeof(rx_msg));
}

void send_to_queue(FDCAN_HandleTypeDef *hfdcan, uint32_t FIFO, FDCAN_RxHeaderTypeDef *header, rx_msg* msg_header, QueueHandle_t queue, TaskHandle_t task)
{
    if(HAL_FDCAN_GetRxMessage(hfdcan,FIFO, header,msg_header->data) == HAL_OK)
    {
        msg_header->id = header->Identifier;
        msg_header->timestamp = header->RxTimestamp;

        if(queue != NULL)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendToBackFromISR(queue, msg_header, &xHigherPriorityTaskWoken);

            UBaseType_t framesWaiting = uxQueueMessagesWaitingFromISR(queue);

            if(framesWaiting >= QUEUE_THRESHOLD && task != NULL)
            {
                vTaskNotifyGiveFromISR(task, &xHigherPriorityTaskWoken);
            }
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef header;
    rx_msg msg_header;
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        if(hfdcan->Instance == FDCAN1)
        {
            send_to_queue(hfdcan, FIFO0, &header, &msg_header, back_msg_queue, process_back_msg);
        }
        else if(hfdcan->Instance == FDCAN2)
        {
            send_to_queue(hfdcan, FIFO0, &header,&msg_header,front_msg_queue, process_front_msg);  
        }
    }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    FDCAN_RxHeaderTypeDef header;
    rx_msg msg_header;
    if ((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != 0)
    {
        if(hfdcan->Instance == FDCAN1)
        {
            send_to_queue(hfdcan, FIFO1, &header, &msg_header, fault_msg_queue, process_fault_msg);
        }
        else if(hfdcan->Instance == FDCAN2)
        {
            send_to_queue(hfdcan, FIFO1, &header, &msg_header, sensor_msg_queue, process_sensor_msg);
        }
    }
}










