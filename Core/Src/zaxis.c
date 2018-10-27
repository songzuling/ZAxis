
#include "zaxis.h"

ZAxis_TypeDef ZAxis;
extern TIM_HandleTypeDef htim4;
extern CAN_HandleTypeDef hcan;

void ZAxis_Init(void)
{
  /*初始化各层对应的高度,单位mm*/
  ZAxis.DistanceOfLayer[0] = 0.f;
  ZAxis.DistanceOfLayer[1] = 450.f;
  ZAxis.DistanceOfLayer[2] = 990.f;
  ZAxis.DistanceOfLayer[3] = 1000.f;
  ZAxis.DistanceOfLayer[4] = 1100.f;
  ZAxis.DistanceOfLayer[5] = 1200.f;
  ZAxis.DistanceOfLayer[6] = 1300.f;
  ZAxis.DistanceOfLayer[7] = 1400.f;
  ZAxis.DistanceOfLayer[8] = 1500.f;
  ZAxis.DistanceOfLayer[ROW_OF_OUTPUT] = 380.f;
  
  /*初始化按键检测*/
  ZAxis.Key[0].Index = 0;
  ZAxis.Key[0].EventOnDoubleClick = DoNothing;
  ZAxis.Key[0].EventOnKeyDown = Test_OnKeyDown0;//Test_DriverMotor;//Test_OnKeyDown0;
  ZAxis.Key[0].EventOnKeyPress = DoNothing;
  ZAxis.Key[0].EventOnKeyUp = DoNothing;
  
  ZAxis.Key[1].Index = 1;
  ZAxis.Key[1].EventOnDoubleClick = DoNothing;
  ZAxis.Key[1].EventOnKeyDown = Test_OnKeyDown1;
  ZAxis.Key[1].EventOnKeyPress = DoNothing;
  ZAxis.Key[1].EventOnKeyUp = DoNothing;
  
  ZAxis.ZeroSwitch.Index = 2;
  ZAxis.ZeroSwitch.EventOnDoubleClick = DoNothing;
  ZAxis.ZeroSwitch.EventOnKeyDown = OnResetSwitchDown;
  ZAxis.ZeroSwitch.EventOnKeyPress = DoNothing;
  ZAxis.ZeroSwitch.EventOnKeyUp =DoNothing;
    
  /*状态初始化*/
  for(int i=0; i < ROW_COUNT; i++)
  {
    for(int j = 0; j < COL_COUNT; j++)
    {
      ZAxis.DeliverCell[i][j] = 0;
    }
  }
  
  /*升降台步进电机初始化*/
  SET_ZAXIS_MOVE_UP;
  ENABLE_ZAXIS_DRIVER;
  SetMoveDirection(ZAXIS_MOVE_UP);
  STOP_ZAXIS;
  ZAxis.CurrentLayer = 0;
  ZAxis.MoveDirection = 0;
  ZAxis.MoveTime = 0;  
  
  /*升降台传动带电机初始化*/
  DISENABLE_MOTOR;
  STOP_MOTOR;  
  
  
  LED1_ON;
  Delay(50);
  LED1_OFF;  
}

/*
 * @brief  Z轴归零
 * @param  空
 * @retval 空
 */
void ResetZaxis(void)
{
  SetMoveDirection(ZAXIS_MOVE_DOWN);    
  START_ZAXIS;
    
  while(ZAxis.ZeroSwitch.IsUp);
  
  ResetState();
}

void ResetState(void)
{
  SET_ZAXIS_MOVE_UP;
  ENABLE_ZAXIS_DRIVER;
  SetMoveDirection(ZAXIS_MOVE_UP);
  STOP_ZAXIS;
  ZAxis.CurrentLayer = 0;
  ZAxis.MoveDirection = 0;
  ZAxis.MoveTime = 0;  
}

/*
 * @brief  将货物从大传动带送到小车
 * @param  空
 * @retval 空
 */
void DeliverGoodsToCar(void)
{
  // TODO:测试，待完善
  ENABLE_MOTOR;
  START_MOTOR;
  
  Delay(10000);
  
  DISENABLE_MOTOR;
  STOP_MOTOR;
}

// 扫描开关
void SwitchScan(void)
{ 
  KeyScan(&(ZAxis.ZeroSwitch),1,HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8));
  KeyScan(&(ZAxis.Key[0]),1,!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0));
  KeyScan(&(ZAxis.Key[1]),1,!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1));  
  return;
}

/*
 * @brief  Z轴主逻辑
 * @param  空
 * @retval 空
 */
void ZAxisMain(void)
{
  /*收到出货消息，需要去接货*/
  if(ZAxis.IsDeliver == CMD_MASTER_DELIVER)
  {      
    LED1_ON;
    for(uint8_t i=0; i<ROW_COUNT; i++)
    {
      /*计算该层是否需要出货*/
      int sum = 0;
      for(int j = 0; j < COL_COUNT; j++)
      {
        sum += ZAxis.DeliverCell[i][j];
      }
      /*该层需要出货*/
      if(sum > 0)
      {                     
        GoToLayer(i);
        
        hcan.pTxMsg->Data[0] = CMD_ZAXIS_REACH;
        hcan.pTxMsg->Data[5] = i;          
        HAL_CAN_Transmit_IT(&hcan);  
        
        if(0 == WaitingForGoods(i, 30000))
        {
          /*出货超时*/
          break;
        }
      }
      else
      {
        continue;
      }
    }// enf for
    LED1_OFF;
    /*接货完成，出口所在层出货*/ 
    GoToLayer(ROW_OF_OUTPUT);  
    /*将货物送到车里*/
    DeliverGoodsToCar();
    /*清空收到的命令*/
    ZAxis.IsDeliver = CMD_NONE;
  }// end if    
}


/*
 * @brief  等待某层的货物出货完毕
 * @param  layer:要出货的那一层
 * @param  timeout:等待时间
 * @retval 0:超时  1:成功
 */
uint8_t WaitingForGoods(int layer, uint32_t timeout)
{
  uint32_t tickstart = HAL_GetTick();
  int sum = 0;  
  do
  {
    sum = 0;
    for(int i = 0; i < COL_COUNT; i++)
    {
      sum += ZAxis.DeliverCell[layer][i];
    }
    if(HAL_GetTick() - tickstart > timeout)
    {
      return 0;
    }
    HAL_Delay(10);
  } while(sum > 0);
  return 1;
}

// 运动到指定的层
void GoToLayer(int8_t layer)
{
  Delay(500);
  GoToDistance(ZAxis.DistanceOfLayer[layer]);
}

void GoToDistance(float toDistance)
{
  int32_t time = DISTANCE_TO_TIME(toDistance);
  /*设定运动方向*/  
  if(ZAxis.MoveTime > time)
  {
    SetMoveDirection(ZAXIS_MOVE_DOWN);    
    START_ZAXIS;
    while( ZAxis.MoveTime > time);
    STOP_ZAXIS;
  }
  else if( ZAxis.MoveTime < time )
  {
    SetMoveDirection(ZAXIS_MOVE_UP);
    START_ZAXIS;
    while( ZAxis.MoveTime < time);
    STOP_ZAXIS;
  }
  else
  {    
    SetMoveDirection(ZAXIS_STOP );
    STOP_ZAXIS;        
  }// end if
}
 


void SetMoveDirection(uint8_t dir)
{
  if(dir == ZAXIS_MOVE_UP)
  {
    SET_ZAXIS_MOVE_UP; 
    ZAxis.MoveDirection = dir - 1;
  }
  else if(dir == ZAXIS_MOVE_DOWN)
  {
    SET_ZAXIS_MOVE_DOWN;
    ZAxis.MoveDirection = dir - 1;
  }
  else 
  {    
    STOP_ZAXIS;    
  }  
  return;
}

void OnResetSwitchDown(int index)
{   
  ResetState();
  return;
}

void DoNothing(int i)
{
  return;
}

void PraseCommand(CAN_HandleTypeDef* hcan)
{    
    uint8_t row = 0;    
    uint8_t col = 0;  
    switch(hcan->pRxMsg->Data[0])
    {
      /*收到出货通知*/
      case CMD_MASTER_DELIVER:
        ZAxis.IsDeliver = CMD_MASTER_DELIVER;   
        row = hcan->pRxMsg->Data[5];        
        col = (hcan->pRxMsg->Data[6]<<8)|hcan->pRxMsg->Data[7];   
        if(row <= (ROW_COUNT-1) && col<=(COL_COUNT-1))
        {          
          ZAxis.DeliverCell[row][col] = 1;
        }
        break;
        
      /*单元体出货完成*/
      case CMD_CELL_DELIVER_DONE:        
        row = hcan->pRxMsg->Data[5];        
        col = (hcan->pRxMsg->Data[6]<<8)|hcan->pRxMsg->Data[7];   
        if(row <= (ROW_COUNT-1) && col<=(COL_COUNT-1))
        {
          ZAxis.DeliverCell[row][col] = 0;
        }
        break;
                
      default:;
      break;
    }    
    return;
}

void UpdateDistance(ZAxis_TypeDef *z)
{
  z->MoveTime += z->MoveDirection;
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{  
    HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
    PraseCommand(hcan);  
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
}

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef *hcan)
{
  HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
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

void HeartBeat(void)
{
  static uint32_t lastTick = 0; 
  if(HAL_GetTick() - lastTick >= 1000)
  {
    lastTick = HAL_GetTick();    
    TOGGLE_LED2;  
    TOGGLE_LED1;
    if(DEBUG_PRINT)SEGGER_RTT_printf(0,"%sMoveTime: %d\r\n",RTT_CTRL_TEXT_BRIGHT_WHITE,ZAxis.MoveTime);
  }
}


void Delay(uint32_t ms)
{
  uint32_t tickstart = HAL_GetTick();
  
  while( HAL_GetTick() - tickstart < ms ){};
  
  return;
}