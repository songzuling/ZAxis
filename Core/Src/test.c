#include "test.h"
extern TIM_HandleTypeDef htim4;

/*
* 测试运动稳定精度
* 
*/
void Test_MoveStable(void)
{    
//  for(uint8_t i = 0; i < 3; i++) 
//  {
//    Delay(1000);
//    GoToLayer(i);
//  } 
//  
  Delay(1000);
  GoToDistance(1000);
  
}

uint8_t moveEnable = 0;
uint8_t moveDir = 0;

void Test_OnKeyDown0(int i)
{
  moveEnable = !moveEnable;
  if(moveEnable == 1)
  {
    START_ZAXIS;
  }
  else
  {
    STOP_ZAXIS;
  }
}

void Test_OnKeyDown1(int i)
{
  if(moveDir == 1)
  {
    SET_ZAXIS_MOVE_UP;
  }
  else
  {
    SET_ZAXIS_MOVE_DOWN;
  }
  moveDir = !moveDir;
}

uint8_t motorEnable = 0;
void Test_DriverMotor(int i)
{
   if(motorEnable == 1)
   {
     ENABLE_MOTOR;
     START_MOTOR;
   }
   else
   {     
     DISENABLE_MOTOR;
     STOP_MOTOR;
   }
   motorEnable = !motorEnable;
}









