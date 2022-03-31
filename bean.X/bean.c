#include "bean.h"

void recBean(BeanData* pBeanData, char bean, unsigned char cnt)
{
  if (cnt >= BEAN_NO_TR_COND /*&& BEANSndSt == BEAN_NO_TR*/) //TRM0_NOTR_COND)
  {
    pBeanData -> recBeanState = (bean ? BEAN_TR_ERR : BEAN_NO_TR);
    return;
  }

  if (pBeanData -> recBeanState == BEAN_NO_TR)
  {
    if (bean == 1)   // This is end of SOF. Init buffer
    {
      // InitBEANVars(1);
      pBeanData -> recBeanState = BEAN_TR_SOF;
    }
    else return;
  }

  if (cnt == 0) return;

  if (pBeanData -> recBeanState == BEAN_TR_SOF)
  {
    cnt--;      // We won't write SOF
    pBeanData -> recBeanState = BEAN_TR_MLINPR;
  }

  if (pBeanData -> recIsNextBitStaffing)
  {
    if (cnt < 6) cnt--;
    pBeanData -> recIsNextBitStaffing = 0;        // Clear bit staffing
  }
  if (cnt == 5) pBeanData -> recIsNextBitStaffing = 1;

  for (; cnt; cnt--)
  {
    pBeanData -> pRecBuffer[pBeanData -> recBuffPos] |= (bean << pBeanData -> recBit);

    if (pBeanData -> recBit == 0)
    {
      if (pBeanData -> recBuffPos == 1)     // We just got first byte. Set ByteCountInTr
      {
        pBeanData -> recBytesCount = pBeanData -> pRecBuffer[1] & 0b00001111;
        pBeanData -> recBeanState = BEAN_TR_ML;
      }
      pBeanData -> recBit = 7;
      pBeanData -> recBuffPos++;
    }
    else pBeanData -> recBit--;
  }
}