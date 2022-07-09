#include "bean.h"
#include <string.h>

void initRecBuffer(RecBeanData *pBeanData)
{
  memset(pBeanData->intBuffer, 0, BEANBUFFSIZE);
  pBeanData->recBit = 7;
  pBeanData->recBuffPos = 0;
  pBeanData->recBeanState = BEAN_NO_TR;
  pBeanData->recIsNextBitStaffing = 0;
}

void recBean(RecBeanData *pBeanData, char bean, unsigned char cnt)
{
  if (cnt >= BEAN_NO_TR_COND)
  {
    // Init buffer
    if (pBeanData->intBuffer == pBeanData->recBuffer1) {
      pBeanData->intBuffer = pBeanData->recBuffer2;
      pBeanData->buffer = pBeanData->recBuffer1;
    } else {
      pBeanData->intBuffer = pBeanData->recBuffer1;
      pBeanData->buffer = pBeanData->recBuffer2;
    }
    initRecBuffer(pBeanData);
    pBeanData->recBeanState = (bean ? BEAN_TR_ERR : BEAN_NO_TR);
    pBeanData->recBufferFull = !bean; // Only full when BEAN_NO_TR
    return;
  }

  if (cnt == 0) return;

  // Use temp variable to define if next bit will be staffing
  // As we may decrease cnt by one on the next step
  unsigned char newNextBitStaffing = (cnt == 5);
  if (pBeanData->recIsNextBitStaffing && cnt < 6) cnt--;
  pBeanData->recIsNextBitStaffing = newNextBitStaffing;


  if (pBeanData->recBeanState == BEAN_NO_TR)
  {
    if (bean == 1) {
      pBeanData->recBeanState = BEAN_TR_IN_PR;
      cnt--; // We won't write SOF
    }
    else return;
  }

  // if (pBeanData->recBeanState == BEAN_TR_SOF)
  // {
  //   cnt--;
  //   pBeanData->recBeanState = BEAN_TR_MLINPR;
  // }

  for (; cnt; cnt--)
  {
    pBeanData->intBuffer[pBeanData->recBuffPos] |= (bean << pBeanData->recBit);

    if (pBeanData->recBit == 0)
    {
      // if (pBeanData->recBuffPos == 1) // We just got first byte. Set ByteCountInTr
      // {
      //   // pBeanData->recBytesCount = pBeanData->intBuffer[0] & 0b00001111;
      //   pBeanData->recBeanState = BEAN_TR_ML;
      // }
      pBeanData->recBit = 7;
      pBeanData->recBuffPos++;
    }
    else
      pBeanData->recBit--;
  }
}

void resetRecBuffer(RecBeanData *pBeanData)
{
  memset(pBeanData->recBuffer1, 0, BEANBUFFSIZE);
  memset(pBeanData->recBuffer2, 0, BEANBUFFSIZE);
  pBeanData->intBuffer = pBeanData->recBuffer1;
  pBeanData->buffer = pBeanData->recBuffer2;
  pBeanData->recBufferFull = 0;
  initRecBuffer(pBeanData);
}
