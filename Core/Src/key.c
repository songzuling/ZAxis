
#include "key.h"


void KeyScan(KeyTypedef *key,uint8_t scanInterval ,uint8_t isKeyPress)
{
  //³õÊ¼»¯°´¼ü¼ì²â
  if(!(key->InitDisable))
  {
    key->DownCnt = 0;
		key->LongPressCnt = 0;
    key->InitDisable = 1;
    key->IsDown = 0;
  }
  
  key->Tick += scanInterval;
  if(isKeyPress)
  {
    key->DownCnt += scanInterval;
    if((key->DownCnt >= 20) && !key->IsDown)
    {
      key->IsDown = 1;
      key->IsUp = 0;
      key->UpCnt = 0;       
      key->ClickCheckFlag = !key->ClickCheckFlag;
      if(key->ClickCheckFlag)
      {
        key->Click_S_Time = key->Tick;
      }
      else
      {
        key->Click_E_Time = key->Tick;
      }      

      if((key->Click_E_Time > key->Click_S_Time)&&((key->Click_E_Time - key->Click_S_Time) > key->CheckAgainInterval) )
      {
        key->EventOnKeyDown(key->Index);
      }
      else if((key->Click_S_Time > key->Click_E_Time)&&((key->Click_S_Time - key->Click_E_Time) > key->CheckAgainInterval) )
      {
        key->EventOnKeyDown(key->Index);
      }	      	
      if((key->Click_E_Time > key->Click_S_Time)&&((key->Click_E_Time - key->Click_S_Time) < 200) )
      {
        key->EventOnDoubleClick(key->Index);
      }
      else if((key->Click_S_Time > key->Click_E_Time)&&((key->Click_S_Time - key->Click_E_Time) < 200) )
      {
        key->EventOnDoubleClick(key->Index);
      }
    }
    if(key->DownCnt >= 400)
		{
			key->IsLongPress = 1;
		}
		if(key->IsLongPress)
		{
			key->LongPressCnt += scanInterval;
			if(key->LongPressCnt >= 80)
			{
				key->EventOnKeyPress(key->Index);
				key->LongPressCnt = 0;				
			}
		}
  }
  else if(!isKeyPress)
  {
    key->UpCnt += scanInterval;
    if(key->UpCnt >= 20)
    {
      key->UpCnt = 20;
      key->IsUp = 1;
      if((key->IsDown))
      {
        key->EventOnKeyUp(key->Index);
      }
      key->DownCnt = 0;
      key->LongPressCnt = 0;
      key->IsDown = 0;
      key->IsLongPress = 0;
    }
  }
  else
  {
  }
  return;
}

