#include <string.h>
#include "bean.h"

void sendBean(SendBeanData *pBeanData)
{
  pBeanData->cnt = 0;

  if (pBeanData->sendBeanState == BEAN_NO_TR_DATA_PRESENT)
  {
    pBeanData->bean = 1; // Send SOF
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

  if (pBeanData->cnt == 0 && (pBeanData->sendBuffer[0] & 0b00001111) + 2 > pBeanData->sendBuffPos)
    pBeanData->bean = !!(pBeanData->sendBuffer[pBeanData->sendBuffPos] & (1 << pBeanData->sentBit));

  while ((pBeanData->sendBuffer[0] & 0b00001111) + 2 > pBeanData->sendBuffPos
         && (pBeanData->bean == (!!(pBeanData->sendBuffer[pBeanData->sendBuffPos] & (1 << pBeanData->sentBit))))
         && pBeanData->cnt < 5)
  {
    if (pBeanData->sentBit == 0)
    {
      pBeanData->sentBit = 7;
      pBeanData->sendBuffPos++;
    }
    else
      pBeanData->sentBit--;
    (pBeanData->cnt)++;
  }

  if (pBeanData->cnt == 5) {
    pBeanData->sendNextBitStaffing = 1;
    return;
  }

    // Data & CRC8 had been sent
  if ((pBeanData->sendBuffer[0] & 0b00001111) + 2 == pBeanData->sendBuffPos)
  {
    switch (pBeanData->sentBit)
    {
    case 7:
      // Last sent bit from CRC can be 1 (bean = 1) In this case skip for this time and send bean = 0 on next iteration
      if (pBeanData->bean == 0 || pBeanData->cnt == 0) {
        pBeanData->sentBit = 6;
        pBeanData->bean = 0;
        pBeanData->cnt++;
      }
      break;
    case 6:
      pBeanData->sentBit = 0;
      pBeanData->bean = 1;
      pBeanData->cnt += 6;
      break;
    case 0:
      pBeanData->sentBit = 6;
      pBeanData->sendBuffPos++;
      pBeanData->bean = 0;
      pBeanData->cnt += 2; // Send last EOM bit and first RSP
      break;
    }
    return;
  }

  // EOM sent, send RSP
  if ((pBeanData->sendBuffer[0] & 0b00001111) + 3 == pBeanData->sendBuffPos)
  {
    switch (pBeanData->sentBit)
    {
    case 6:
      pBeanData->sentBit--;
      pBeanData->bean = 1;
      pBeanData->cnt += 1;
      break;
    case 5:
      pBeanData->sendBeanState = BEAN_NO_TR;
      pBeanData->bean = 0;
      pBeanData->cnt = 0;
      break;
    }
    return;
  }
}

unsigned char initSendBeanData(SendBeanData *pBeanData, unsigned char *buff)
{
  // All other states, except BEAN_NO_TR does not allow to put data into buffer
  if (pBeanData->sendBeanState != BEAN_NO_TR) return 0;
  resetSendBuffer(pBeanData);
  // Add 1 byte for ML and one for CRC
  memcpy(pBeanData->sendBuffer, buff, (buff[0] & 0x0F) + 2);
  pBeanData->sendBeanState = BEAN_NO_TR_DATA_PRESENT;
  return 1;
}

void resetSendBuffer(SendBeanData *pBeanData) {
  pBeanData->sendBuffer[0] = 0;
  pBeanData->sentBit = 7;
  pBeanData->sendBuffPos = 0;
  pBeanData->cnt = 0;
  pBeanData->sendBeanState = BEAN_NO_TR;

  pBeanData->sendNextBitStaffing = 0;
  pBeanData->bean = 0;
}

unsigned char isTransferInProgress(SendBeanData *pBeanData) {
  return pBeanData->sendBeanState != BEAN_NO_TR
          && pBeanData->sendBeanState != BEAN_TR_ERR
          && pBeanData->sendBeanState != BEAN_NO_TR_DATA_PRESENT;
}

unsigned char canStartTransfer(BeanTransferState sendBeanState, BeanTransferState recBeanState)
{
  return sendBeanState == BEAN_NO_TR_DATA_PRESENT && recBeanState == BEAN_NO_TR;
}