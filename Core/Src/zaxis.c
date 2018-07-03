
#include "zaxis.h"

ZAxis_TypeDef ZAxis;

void ZAxis_Init(void)
{
  
  /*初始化各层开关检测*/
  for(int i=0; i<LAYER_OF_CELL; i++)
  {
    ZAxis.Layer[i].Index = i;
    ZAxis.Layer[i].OnDoubleClickEnable = 0;
    ZAxis.Layer[i].OnKeyDownEnable = 1;
    ZAxis.Layer[i].OnKeyPressEnable = 0;
    ZAxis.Layer[i].OnKeyUpEnable = 1;
    ZAxis.Layer[i].EventOnDoubleClick = DoNothing;
    ZAxis.Layer[i].EventOnKeyDown = OnSwitchDown;
    ZAxis.Layer[i].EventOnKeyPress = DoNothing;
    ZAxis.Layer[i].EventOnKeyUp = DoNothing;
  }
}

void SwitchScan(void)
{
  KeyScan(&(ZAxis.Layer[0]),1,!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));
  KeyScan(&(ZAxis.Layer[1]),1,!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9));
  KeyScan(&(ZAxis.Layer[2]),1,!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10));  
  return;
}

void Control(void)
{

}
    
void OnSwitchDown(int index)
{
  return;
}
    
void DoNothing(int i)
{
  return;
}

void CheckCommand(CAN_HandleTypeDef* hcan)
{
    switch(hcan->pRxMsg->Data[0])
    {

      default:;
      break;
    }    
    return;
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{  
    HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
    CheckCommand(hcan);  
}

void SetDuty(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t duty)
{
    uint32_t tmp = 0;
    if(duty > 99)
    {
        duty = 99;
    }
    tmp = htim->Init.Period/100*duty;
    switch(Channel)
    {
      case TIM_CHANNEL_1:htim->Instance->CCR1 = tmp;break;
      case TIM_CHANNEL_2:htim->Instance->CCR2 = tmp;break;
      case TIM_CHANNEL_3:htim->Instance->CCR3 = tmp;break;
      case TIM_CHANNEL_4:htim->Instance->CCR4 = tmp;break;
      default:break;
    }
    return;
}
