
#ifndef __ZAXIS_H__
#define __ZAXIS_H__


#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "string.h"
#include "key.h"

#define LAYER_OF_CELL  9

typedef struct
{
  uint8_t     LayerDeliverCellCount[LAYER_OF_CELL]; /*ÿ����Ҫ�����ĵ�Ԫ������*/
  KeyTypedef  Layer[LAYER_OF_CELL];                 /*������г̿���*/
}ZAxis_TypeDef;

enum{
  CELL_NONE = 0U,
  CELL_RESET,                   /*��λ��Ԫ��*/
  CELL_RESET_SYSTEM,            /*��λ��Ԫ��ϵͳ*/
  CELL_REQUEST_ENABLE,          /*ʹ������ID*/
  CELL_REQUEST_ID,              /*�������ID*/
  CELL_ALLOCATE_ONE_LAYER_DONE, /*����һ�����*/
  CELL_GET_ID,                  /*�յ�ID*/
  CELL_DELIVER,                 /*��������*/
  CELL_DELIVER_DONE,            /*��Ԫ��������*/
};

extern ZAxis_TypeDef ZAxis;


void ZAxis_Init(void);
void DoNothing(int i);
void OnSwitchDown(int index);
void SwitchScan(void);
void CheckCommand(CAN_HandleTypeDef* hcan);
void SetDuty(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t duty);
#endif