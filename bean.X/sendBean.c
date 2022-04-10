#include "bean.h"

void sendBean(SendBeanData *pBeanData)
{
  pBeanData->cnt = 0;

  if (pBeanData->sendBeanState == BEAN_NO_TR) {
    pBeanData->bean = 1;  // Send SOF
    pBeanData->sendBeanState = BEAN_TR_SOF;
    pBeanData->cnt = 1;
  }

  // Staffing bit has been requested from previous transfer
  if (pBeanData->sendNextBitStaffing)
  {
    pBeanData->bean = !(pBeanData->bean);
    pBeanData->cnt = 1;
    pBeanData->sendNextBitStaffing = 0;
  }

  // Data & CRC8 had been sent
  if ((pBeanData->sendBuffer[0] & 0b00001111) + 2 == pBeanData->sendBuffPos
    && pBeanData->cnt == 0)
  {
      pBeanData->sendBeanState = BEAN_TR_EOM;
      switch (pBeanData->sentBit)
      {
        case 7:
          pBeanData->sentBit = 6;
          pBeanData->bean = 0;
          pBeanData->cnt = 1;
          break;
        case 6:
          pBeanData->sentBit = 0;
          pBeanData->bean = 1;
          pBeanData->cnt = 6;
          break;
        case 0:
          pBeanData->sentBit = 6;
          pBeanData->sendBuffPos++;
          pBeanData->bean = 0;
          pBeanData->cnt = 2; // Send last EOM bit and first RSP
          break;
      }
    return;
  }

  // EOM sent, send RSP
  if ((pBeanData->sendBuffer[0] & 0b00001111) + 3 == pBeanData->sendBuffPos
    && pBeanData->cnt == 0)
  {
      pBeanData->sendBeanState = BEAN_TR_RSP;
      switch (pBeanData->sentBit)
      {
        case 6:
          pBeanData->sentBit--;
          pBeanData->bean = 1;
          pBeanData->cnt = 1;
          break;
        case 5:
          pBeanData->sendBeanState = BEAN_NO_TR;
          pBeanData->bean = 0;
          pBeanData->cnt = 0;
          break;
      }
    return;
  }

  if (pBeanData->cnt == 0)
    pBeanData->bean = !!(pBeanData->sendBuffer[pBeanData->sendBuffPos] & (1 << pBeanData->sentBit));

  while (((pBeanData->sendBuffer[0] & 0b00001111) + 2 != pBeanData->sendBuffPos) &&
         (pBeanData->bean == (!!(pBeanData->sendBuffer[pBeanData->sendBuffPos] & (1 << pBeanData->sentBit)))) &&
         pBeanData->cnt != 5)
  {
    if (pBeanData->sentBit == 0)
    {
      pBeanData->sentBit = 7;
      pBeanData->sendBuffPos++;
    }
    else pBeanData->sentBit--;
    (pBeanData->cnt)++;
  }

  if (pBeanData->cnt == 5)
    pBeanData->sendNextBitStaffing = 1;
}