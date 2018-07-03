
#ifndef __KEY_H__
#define __KEY_H__


#include "stm32f1xx_hal.h"
#include "cmsis_os.h"


typedef struct
{
  int     Index;
  uint8_t InitDisable;
  uint8_t IsDown;
	uint8_t IsLongPress;
	uint8_t ClickCheckFlag;
  
	uint8_t OnKeyDownEnable;
	uint8_t OnKeyUpEnable;
  uint8_t OnKeyPressEnable;
	uint8_t OnDoubleClickEnable;
  
	void (*EventOnKeyDown)(int index);
  void (*EventOnKeyUp)(int index);
	void (*EventOnKeyPress)(int index);
	void (*EventOnDoubleClick)(int index);
	uint32_t Click_S_Time;
	uint32_t Click_E_Time;
	uint32_t Tick;
  uint32_t DownCnt;
  uint32_t UpCnt;
	uint32_t LongPressCnt;
  uint32_t CheckAgainInterval;
}KeyTypedef;

void KeyScan(KeyTypedef *key,uint8_t scanInterval ,uint8_t isKeyPress);

#endif