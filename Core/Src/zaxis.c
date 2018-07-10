
#include "zaxis.h"

ZAxis_TypeDef ZAxis;
extern TIM_HandleTypeDef htim4;

void ZAxis_Init(void)
{
  
  /*初始化各层开关检测*/
  for(int i=0; i<SWITCH_COUNT; i++)
  {
    ZAxis.LayerSwitch[i].Index = i;
    ZAxis.LayerSwitch[i].OnDoubleClickEnable = 0;
    ZAxis.LayerSwitch[i].OnKeyDownEnable = 1;
    ZAxis.LayerSwitch[i].OnKeyPressEnable = 0;
    ZAxis.LayerSwitch[i].OnKeyUpEnable = 1;
    ZAxis.LayerSwitch[i].EventOnDoubleClick = DoNothing;
    ZAxis.LayerSwitch[i].EventOnKeyDown = OnLayerSwitchDown;
    ZAxis.LayerSwitch[i].EventOnKeyPress = DoNothing;
    ZAxis.LayerSwitch[i].EventOnKeyUp = DoNothing;
  }
  
  
  
}

// 扫描开关
void SwitchScan(void)
{
  KeyScan(&(ZAxis.LayerSwitch[1]),1,!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));
  KeyScan(&(ZAxis.LayerSwitch[2]),1,!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9));
  KeyScan(&(ZAxis.LayerSwitch[3]),1,!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10));  
  return;
}

uint8_t getlayer = 0;
void ZAxisMain(void)
{
  uint8_t shouldexit = 0;
  while(!shouldexit)
  {
    /*收到出货消息，需要去接货*/
    if(ZAxis.IsDeliver == CMD_MASTER_DELIVER)
    {
      ZAxis.IsDeliver = CMD_NONE;
      for(uint8_t i=0; i<SWITCH_COUNT; i++)
      {
        /*该层需要出货*/
        if(ZAxis.DeliverCellCount[i] > 0)
        {
          GoToLayer(ZAxis.CurrentLayer, i);
          getlayer = i;
//          if(0 == WaitingForGoods(i, 20000))
//          {
//            /*出货超时*/
//            break;
//          }
        }
        else
        {
          continue;
        }
      }// enf for
      
      /*接货完成，出口所在层出货*/ 
      GoToLayer(ZAxis.CurrentLayer, LAYER_OF_OUTPUT);
    }// end if
  }
}

// 等待某层的货物出货完毕
uint8_t WaitingForGoods(int layer, uint32_t timeout)
{
  uint32_t tickstart = HAL_GetTick();
  while(ZAxis.DeliverCellCount[layer] > 0)
  {
    if(HAL_GetTick() - tickstart > timeout)
    {
      return 0;
    }
  }
  return 1;
}

// 运动到指定的层
void GoToLayer(int curLayer, int layer)
{
  /*设定运动方向*/  
  if(curLayer - layer > 0)
  {
    SetMoveDirection(ZAXIS_MOVE_DOWN);
  }
  else if(curLayer - layer < 0)
  {
    SetMoveDirection(ZAXIS_MOVE_UP);
  }
  /*要去层等于当前层，可能正在当前层，也可能在
    当前层上面或者下面未到达其他层的地方*/
  else
  {
    /*之前向上运动，现在向下*/
    if(ZAxis.MoveDirection == ZAXIS_MOVE_UP)
    {
      SetMoveDirection(ZAXIS_MOVE_DOWN);
    }
    /*之前向下运动，现在向上*/
    else if(ZAxis.MoveDirection == ZAXIS_MOVE_DOWN)
    {
      SetMoveDirection(ZAXIS_MOVE_UP);
    }
    else
    {
      
    }
  }// end if
      /**/
  while(ZAxis.LayerSwitch[layer].IsDown == 0)
  { 
    ///////////////////////////////////////////////
    //注意添加运动过程中触底的情况判断
    ///////////////////////////////////////////////
    
    SetDuty(&htim4,TIM_CHANNEL_1,50);
    SetDuty(&htim4,TIM_CHANNEL_2,50);
    SetDuty(&htim4,TIM_CHANNEL_3,50);
    SetDuty(&htim4,TIM_CHANNEL_4,50);         
  }
  SetDuty(&htim4,TIM_CHANNEL_1,0);
  SetDuty(&htim4,TIM_CHANNEL_2,0);
  SetDuty(&htim4,TIM_CHANNEL_3,0);
  SetDuty(&htim4,TIM_CHANNEL_4,0);    
  
}
    
void SetMoveDirection(uint8_t dir)
{
  if(dir == ZAXIS_MOVE_UP)
  {
    /*控制引脚改变运动方向*/
       
  }
  else if(dir == ZAXIS_MOVE_DOWN)
  {
  
  }
  else
  {
  
  }    
  /*改变运动方向标志*/
  ZAxis.MoveDirection = dir;
  
  return;
}

// 相应层的行程开关白按下时条用
void OnLayerSwitchDown(int index)
{
  /*更新当前层数*/
  ZAxis.CurrentLayer = index;
  return;
}

void DoNothing(int i)
{
  return;
}

void PraseCommand(CAN_HandleTypeDef* hcan)
{    
    uint8_t row = 0;
    switch(hcan->pRxMsg->Data[0])
    {
      /*收到出货通知*/
      case CMD_MASTER_DELIVER:
        ZAxis.IsDeliver = CMD_MASTER_DELIVER;   
        row = hcan->pRxMsg->Data[5];
        if(row <= SWITCH_COUNT)
        {
          if(row == LAYER_OF_OUTPUT)
          {
            row++;
          }
          ZAxis.DeliverCellCount[row]++;
        }
        break;
        
      /*单元体出货完成*/
      case CMD_CELL_DELIVER_DONE:        
        row = hcan->pRxMsg->Data[5];
        if(row <= SWITCH_COUNT)
        {
          if(row == LAYER_OF_OUTPUT)
          {
            row++;
          }
          ZAxis.DeliverCellCount[row]--;
        }
        break;
                
      default:;
      break;
    }    
    return;
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{  
    HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
    PraseCommand(hcan);  
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
