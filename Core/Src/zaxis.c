
#include "zaxis.h"

ZAxis_TypeDef ZAxis;
extern TIM_HandleTypeDef htim4;

void ZAxis_Init(void)
{
  
  /*��ʼ�����㿪�ؼ��*/
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

// ɨ�迪��
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
    /*�յ�������Ϣ����Ҫȥ�ӻ�*/
    if(ZAxis.IsDeliver == CMD_MASTER_DELIVER)
    {
      ZAxis.IsDeliver = CMD_NONE;
      for(uint8_t i=0; i<SWITCH_COUNT; i++)
      {
        /*�ò���Ҫ����*/
        if(ZAxis.DeliverCellCount[i] > 0)
        {
          GoToLayer(ZAxis.CurrentLayer, i);
          getlayer = i;
//          if(0 == WaitingForGoods(i, 20000))
//          {
//            /*������ʱ*/
//            break;
//          }
        }
        else
        {
          continue;
        }
      }// enf for
      
      /*�ӻ���ɣ��������ڲ����*/ 
      GoToLayer(ZAxis.CurrentLayer, LAYER_OF_OUTPUT);
    }// end if
  }
}

// �ȴ�ĳ��Ļ���������
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

// �˶���ָ���Ĳ�
void GoToLayer(int curLayer, int layer)
{
  /*�趨�˶�����*/  
  if(curLayer - layer > 0)
  {
    SetMoveDirection(ZAXIS_MOVE_DOWN);
  }
  else if(curLayer - layer < 0)
  {
    SetMoveDirection(ZAXIS_MOVE_UP);
  }
  /*Ҫȥ����ڵ�ǰ�㣬�������ڵ�ǰ�㣬Ҳ������
    ��ǰ�������������δ����������ĵط�*/
  else
  {
    /*֮ǰ�����˶�����������*/
    if(ZAxis.MoveDirection == ZAXIS_MOVE_UP)
    {
      SetMoveDirection(ZAXIS_MOVE_DOWN);
    }
    /*֮ǰ�����˶�����������*/
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
    //ע������˶������д��׵�����ж�
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
    /*�������Ÿı��˶�����*/
       
  }
  else if(dir == ZAXIS_MOVE_DOWN)
  {
  
  }
  else
  {
  
  }    
  /*�ı��˶������־*/
  ZAxis.MoveDirection = dir;
  
  return;
}

// ��Ӧ����г̿��ذװ���ʱ����
void OnLayerSwitchDown(int index)
{
  /*���µ�ǰ����*/
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
      /*�յ�����֪ͨ*/
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
        
      /*��Ԫ��������*/
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
