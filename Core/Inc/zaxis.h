
#ifndef __ZAXIS_H__
#define __ZAXIS_H__


#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "string.h"
#include "key.h"

#define LAYER_OF_CELL  9

typedef struct
{
  uint8_t     LayerDeliverCellCount[LAYER_OF_CELL]; /*每层需要出货的单元体数量*/
  KeyTypedef  Layer[LAYER_OF_CELL];                 /*各层的行程开关*/
}ZAxis_TypeDef;

enum{
  CELL_NONE = 0U,
  CELL_RESET,                   /*复位单元体*/
  CELL_RESET_SYSTEM,            /*复位单元体系统*/
  CELL_REQUEST_ENABLE,          /*使能请求ID*/
  CELL_REQUEST_ID,              /*请求分配ID*/
  CELL_ALLOCATE_ONE_LAYER_DONE, /*分配一层完成*/
  CELL_GET_ID,                  /*收到ID*/
  CELL_DELIVER,                 /*出货命令*/
  CELL_DELIVER_DONE,            /*单元体出货完成*/
};

extern ZAxis_TypeDef ZAxis;


void ZAxis_Init(void);
void DoNothing(int i);
void OnSwitchDown(int index);
void SwitchScan(void);
void CheckCommand(CAN_HandleTypeDef* hcan);
void SetDuty(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t duty);
#endif