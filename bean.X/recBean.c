#include "bean.h"
#include <string.h>

void initRecBuffer(RecBeanData *pBeanData, unsigned char bufIdx)
{
  memset(pBeanData->recBuffer[bufIdx], 0, BEANBUFFSIZE);
  pBeanData->currRecBufferIdx = bufIdx;
  pBeanData->recBit = 7;
  pBeanData->recBuffPos = 0;
  pBeanData->recBytesCount = 0;
  pBeanData->recBeanState = BEAN_NO_TR;
  pBeanData->recIsNextBitStaffing = 0;
}

void recBean(RecBeanData *pBeanData, char bean, unsigned char cnt)
{
  if (cnt >= BEAN_NO_TR_COND)
  {
    // Init buffer
    if (pBeanData->currRecBufferIdx == 2)
      pBeanData->currRecBufferIdx = 0;
    else
      pBeanData->currRecBufferIdx++;
    initRecBuffer(pBeanData, pBeanData->currRecBufferIdx);
    pBeanData->recBeanState = (bean ? BEAN_TR_ERR : BEAN_NO_TR);
    return;
  }

  if (pBeanData->recBeanState == BEAN_NO_TR)
  {
    if (bean == 1)
    {
      pBeanData->recBeanState = BEAN_TR_SOF;
    }
    else
      return;
  }

  if (cnt == 0)
    return;

  if (pBeanData->recBeanState == BEAN_TR_SOF)
  {
    cnt--; // We won't write SOF
    pBeanData->recBeanState = BEAN_TR_MLINPR;
  }

  if (pBeanData->recIsNextBitStaffing)
  {
    if (cnt < 6)
      cnt--;
    pBeanData->recIsNextBitStaffing = 0; // Clear bit staffing
  }
  if (cnt == 5)
    pBeanData->recIsNextBitStaffing = 1;

  for (; cnt; cnt--)
  {
    pBeanData->recBuffer[pBeanData->currRecBufferIdx][pBeanData->recBuffPos] |= (bean << pBeanData->recBit);

    if (pBeanData->recBit == 0)
    {
      if (pBeanData->recBuffPos == 1) // We just got first byte. Set ByteCountInTr
      {
        pBeanData->recBytesCount = pBeanData->recBuffer[pBeanData->currRecBufferIdx][1] & 0b00001111;
        pBeanData->recBeanState = BEAN_TR_ML;
      }
      pBeanData->recBit = 7;
      pBeanData->recBuffPos++;
    }
    else
      pBeanData->recBit--;
  }
}

void resetRecBuffer(RecBeanData *pBeanData)
{
  memset(pBeanData->recBuffer[0], 0, BEANBUFFSIZE);
  memset(pBeanData->recBuffer[1], 0, BEANBUFFSIZE);
  memset(pBeanData->recBuffer[2], 0, BEANBUFFSIZE);
  initRecBuffer(pBeanData, 0);
}
