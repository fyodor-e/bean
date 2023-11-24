#include "bean.h"
#include <string.h>

void initRecBuffer(RecBeanData *pBeanData, unsigned char isError, unsigned char isEOM)
{
  if (!isError) {
    if (pBeanData->recBeanState == BEAN_TR_GOT_EOM) {
      if (!isEOM) pBeanData->recBeanState = BEAN_NO_TR;
      return;
    }
    if (pBeanData->recBeanState == BEAN_NO_TR) return;

    // Rotate buffer
    if (pBeanData->intBuffer == pBeanData->recBuffer1) {
      pBeanData->intBuffer = pBeanData->recBuffer2;
      pBeanData->buffer = pBeanData->recBuffer1;
    } else {
      pBeanData->intBuffer = pBeanData->recBuffer1;
      pBeanData->buffer = pBeanData->recBuffer2;
    }
    memset(pBeanData->intBuffer, 0, BEANBUFFSIZE);
    pBeanData->recBit = 7;
    pBeanData->recBuffPos = 0;
    pBeanData->recBeanState = isEOM ? BEAN_TR_GOT_EOM : BEAN_NO_TR;
    pBeanData->recIsNextBitStaffing = 0;
    // Always set to full event if previous reception was erroneous
    // PC app will get erroneous reception to display it to user
    pBeanData->recBufferFull = 1;
  } else {
    if (pBeanData->recBeanState == BEAN_TR_ERR) return;

    pBeanData->recBeanState = BEAN_TR_ERR;
  }
}

void recBean(RecBeanData *pBeanData, char bean, unsigned char cnt)
{
  // If there is no transfer cond - reset error and init buffers
  if (cnt >= BEAN_NO_TR_COND)
  {
    // Error condition if bean is high during No Transfer Condition
    initRecBuffer(pBeanData, bean, 0);
    return;
  }

  // If transfer still in error condition - exit
  if (cnt == 0 || pBeanData->recBeanState == BEAN_TR_ERR) return;

  // Use temp variable to define if next bit will be staffing
  // As we may decrease cnt by one on the next step
  unsigned char newNextBitStaffing = (cnt == 5);
  if (pBeanData->recIsNextBitStaffing && cnt < 6) cnt--;
  pBeanData->recIsNextBitStaffing = newNextBitStaffing;

  if (pBeanData->recBeanState == BEAN_NO_TR || pBeanData->recBeanState == BEAN_TR_GOT_EOM)
  {
    if (bean == 1) {
      pBeanData->recBeanState = BEAN_TR_IN_PR;
      cnt--; // We won't write SOF
    }
    else {
      pBeanData->recIsNextBitStaffing = 0;
      return;
    }
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

  // At least first byte is received
  if (pBeanData->recBuffPos > 0
    // Whole message was received + CRC and RSP.
    // We're receiving EOM
    && (pBeanData->intBuffer[0] & 0x0F) + 3 == pBeanData->recBuffPos
    // We're on EOF
    && pBeanData->recBit == 5
    // We've got EOM
    && pBeanData->intBuffer[pBeanData->recBuffPos - 1] == 0x7E
    // And RSP
    && pBeanData->intBuffer[pBeanData->recBuffPos] == 0x40) {
    initRecBuffer(pBeanData, 0, 1);
  }

  if (pBeanData->recBuffPos >= BEANBUFFSIZE) initRecBuffer(pBeanData, 1, 0);
}

void resetRecBuffer(RecBeanData *pBeanData)
{
  memset(pBeanData->recBuffer1, 0, BEANBUFFSIZE);
  memset(pBeanData->recBuffer2, 0, BEANBUFFSIZE);
  pBeanData->intBuffer = pBeanData->recBuffer1;
  pBeanData->buffer = pBeanData->recBuffer2;
  pBeanData->recBufferFull = 0;
  pBeanData->recBit = 7;
  pBeanData->recBuffPos = 0;
  pBeanData->recBeanState = BEAN_NO_TR;
  pBeanData->recIsNextBitStaffing = 0;
}

#ifdef __32MX220F032B__
unsigned char getCntFromTmr(unsigned short tmr, unsigned short t_cnt)
{
  unsigned char cnt = tmr / t_cnt;
  unsigned char reminder = tmr - (cnt * t_cnt);
  return cnt + (reminder > (t_cnt * 3) / 4);
}
#endif