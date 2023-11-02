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
  // If there is no transfer cond - reset error and init buffers
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

  // If transfer still in error condition - exit
  if (cnt == 0 || pBeanData->recBeanState == BEAN_TR_ERR) return;

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

  // We should not receive more than BEANBUFFSIZE
  for (; cnt && pBeanData->recBuffPos < BEANBUFFSIZE; cnt--)
  {
    pBeanData->intBuffer[pBeanData->recBuffPos] |= (bean << pBeanData->recBit);

    if (pBeanData->recBit == 0)
    {
      pBeanData->recBit = 7;
      pBeanData->recBuffPos++;
    }
    else
      pBeanData->recBit--;
  }

  if (pBeanData->recBuffPos >= BEANBUFFSIZE) pBeanData->recBeanState = BEAN_TR_ERR;
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

#ifdef __32MX220F032B__
unsigned char getCntFromTmr(unsigned short tmr, unsigned short t_cnt)
{
  unsigned char cnt = tmr / t_cnt;
  unsigned char reminder = tmr - (cnt * t_cnt);
  return cnt + (reminder > (t_cnt * 3) / 4);
}
#endif