#include <gtest/gtest.h>
#define __32MX220F032B__

#include "bean.X/recBean.c"

// Instruction
// https://stackoverflow.com/questions/62910867/how-to-run-tests-and-debug-google-test-project-in-vs-code

typedef struct
{
  unsigned char *pData;
  int dataSize;
  unsigned char byteTr;
  unsigned char bitTr;
  unsigned char bean;
  // staffing bit should be sent first
  unsigned char cnt;
  unsigned char trInPr;
} BeanTransfer;

class BeanTestClass : public ::testing::Test
{
protected:
  RecBeanData beanData;
  bool getNextData(BeanTransfer &beanTransfer)
  {
    if (beanTransfer.trInPr == 0)
    {
      // We're starting transmission, set SOF
      beanTransfer.cnt = 1;
      beanTransfer.bean = 1;
      // Set up bit tr to first bit
      beanTransfer.bitTr = 7;
      beanTransfer.trInPr = 1;
    }
    else
    {
      // If prev cnt was 5 add staffing bit
      beanTransfer.cnt = (beanTransfer.cnt == 5 ? 1 : 0);
      beanTransfer.bean = !beanTransfer.bean;
    }

    if (beanTransfer.byteTr < beanTransfer.dataSize)
    {
      while (beanTransfer.byteTr < beanTransfer.dataSize
             // Either stop on 5 conseq bits with same val (to add staffing)
             // or ignore this case when sending EOF or last byte
             && (beanTransfer.cnt < 5 || beanTransfer.byteTr >= beanTransfer.dataSize - 2)
             && !(beanTransfer.pData[beanTransfer.byteTr] & (0b1 << beanTransfer.bitTr)) == !beanTransfer.bean)
      {
        if (beanTransfer.bitTr == 0)
        {
          beanTransfer.bitTr = 7;
          beanTransfer.byteTr++;
        }
        else
          beanTransfer.bitTr--;
        beanTransfer.cnt++;
      }
    }
    return beanTransfer.byteTr < beanTransfer.dataSize;
  }

  void initBeanTransfer(BeanTransfer *pbeanTransfer, unsigned char *data, unsigned char size)
  {
    pbeanTransfer->pData = data;
    // byteTr and bitTr should be set to 0 to init transfer. See getNextData
    pbeanTransfer->byteTr = 0;
    pbeanTransfer->bitTr = 0;
    pbeanTransfer->bean = 0;
    // staffing bit should be sent first
    pbeanTransfer->cnt = 0;
    pbeanTransfer->trInPr = 0;
    pbeanTransfer->dataSize = size;
  }

  virtual void SetUp()
  {
    resetRecBuffer(&beanData);
  }
};

TEST_F(BeanTestClass, Should_reset_rec_bean_data)
{
  EXPECT_EQ(beanData.recBuffer1[0], 0);
  EXPECT_EQ(beanData.recBuffer2[0], 0);
  EXPECT_EQ(beanData.intBuffer, beanData.recBuffer1);
  EXPECT_EQ(beanData.buffer, beanData.recBuffer2);
  EXPECT_EQ(beanData.recBit, 7);
  EXPECT_EQ(beanData.recBuffPos, 0);
  EXPECT_EQ(beanData.recBeanState, BEAN_NO_TR);
  EXPECT_EQ(beanData.recIsNextBitStaffing, 0);
  EXPECT_EQ(beanData.recBufferFull, 0);
}

TEST_F(BeanTestClass, Calling_initRecBuffer_twice_should_not_change_beanRecState)
{
  initRecBuffer(&beanData, 0);

  EXPECT_EQ(beanData.intBuffer, beanData.recBuffer1);
  EXPECT_EQ(beanData.buffer, beanData.recBuffer2);
  EXPECT_EQ(beanData.recBeanState, BEAN_NO_TR);
  EXPECT_EQ(beanData.recBufferFull, 0);

  initRecBuffer(&beanData, 0);

  EXPECT_EQ(beanData.intBuffer, beanData.recBuffer1);
  EXPECT_EQ(beanData.buffer, beanData.recBuffer2);
  EXPECT_EQ(beanData.recBeanState, BEAN_NO_TR);
  EXPECT_EQ(beanData.recBufferFull, 0);
}

TEST_F(BeanTestClass, Should_Set_BEAN_NO_TR_When_More_Than_BEAN_NO_TR_COND_bits_got)
{
  // In case of 0
  beanData.recBeanState = BEAN_TR_DATA;
  recBean(&beanData, 0, BEAN_NO_TR_COND);
  EXPECT_EQ(beanData.recBeanState, BEAN_NO_TR);

  beanData.recBeanState = BEAN_TR_DATA;
  recBean(&beanData, 1, BEAN_NO_TR_COND);
  EXPECT_EQ(beanData.recBeanState, BEAN_TR_ERR);
}

TEST_F(BeanTestClass, Should_Accept_Simple_Transfer_WO_Staffing)
{
  unsigned char data[] = {0b00010100, 0b10010010, 0b00010001, 0b00010001, 0b00100010, 0b00110011, 0b01111110, 0b01000000};
  BeanTransfer beanTransfer;
  initBeanTransfer(&beanTransfer, data, sizeof(data) / sizeof(unsigned char));
  beanData.recBeanState = BEAN_NO_TR;

  //while (getNextData(beanTransfer))

  recBean(&beanData, 0, 0); // Rising edge of staffing
  recBean(&beanData, 1, 1); // staffing
  recBean(&beanData, 0, 3);
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, 1);
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, 2); // first byte ends
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, 2);
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, 2);
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, 4); // second byte ends
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, 3);
  recBean(&beanData, 1, 1); // third byte ends
  recBean(&beanData, 0, 3);
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, 3);
  recBean(&beanData, 1, 1); // forth byte ends
  recBean(&beanData, 0, 2);
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, 3);
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, 3); // fifth byte ends
  recBean(&beanData, 1, 2);
  recBean(&beanData, 0, 2);
  recBean(&beanData, 1, 2); // sixth byte ends
  recBean(&beanData, 0, 1);
  recBean(&beanData, 1, 6);
  recBean(&beanData, 0, 2);
  recBean(&beanData, 1, 1);
  recBean(&beanData, 0, BEAN_NO_TR_COND);

  EXPECT_EQ(beanData.recBufferFull, 1);
  EXPECT_EQ(beanData.buffer[0], data[0]);
  EXPECT_EQ(beanData.buffer[1], data[1]);
  EXPECT_EQ(beanData.buffer[2], data[2]);
  EXPECT_EQ(beanData.buffer[3], data[3]);
  EXPECT_EQ(beanData.buffer[4], data[4]);
  EXPECT_EQ(beanData.buffer[5], data[5]);
  EXPECT_EQ(beanData.buffer[6], data[6]);
  // EXPECT_EQ(beanData.buffer[7], data[7]); ?????
}

TEST_F(BeanTestClass, Should_Accept_Transfer_With_Staffing)
{
  unsigned char data[] = {0b00010100, 0b00010000, 0b00000001, 0b00011111, 0b11100010, 0b10111111, 0b01111110, 0b01000000};
  BeanTransfer beanTransfer;
  initBeanTransfer(&beanTransfer, data, sizeof(data) / sizeof(unsigned char));
  beanData.recBeanState = BEAN_NO_TR;

  while (getNextData(beanTransfer))
    recBean(&beanData, beanTransfer.bean, beanTransfer.cnt);
  recBean(&beanData, 0, BEAN_NO_TR_COND);

  EXPECT_EQ(beanData.recBufferFull, 1);
  EXPECT_EQ(beanData.buffer[0], data[0]);
  EXPECT_EQ(beanData.buffer[1], data[1]);
  EXPECT_EQ(beanData.buffer[2], data[2]);
  EXPECT_EQ(beanData.buffer[3], data[3]);
  EXPECT_EQ(beanData.buffer[4], data[4]);
  EXPECT_EQ(beanData.buffer[5], data[5]);
  EXPECT_EQ(beanData.buffer[6], data[6]);
  EXPECT_EQ(beanData.buffer[7], data[7]);
}

TEST_F(BeanTestClass, Should_Accept_Transfer_With_00andFF)
{
  unsigned char data[] = {0x06, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0x42, 0b01111110, 0b01000000};
  BeanTransfer beanTransfer;
  initBeanTransfer(&beanTransfer, data, sizeof(data) / sizeof(unsigned char));
  beanData.recBeanState = BEAN_NO_TR;

  while (getNextData(beanTransfer))
    recBean(&beanData, beanTransfer.bean, beanTransfer.cnt);
  recBean(&beanData, 0, BEAN_NO_TR_COND);

  EXPECT_EQ(beanData.recBufferFull, 1);
  EXPECT_EQ(beanData.buffer[0], data[0]);
  EXPECT_EQ(beanData.buffer[1], data[1]);
  EXPECT_EQ(beanData.buffer[2], data[2]);
  EXPECT_EQ(beanData.buffer[3], data[3]);
  EXPECT_EQ(beanData.buffer[4], data[4]);
  EXPECT_EQ(beanData.buffer[5], data[5]);
  EXPECT_EQ(beanData.buffer[6], data[6]);
  EXPECT_EQ(beanData.buffer[7], data[7]);
  EXPECT_EQ(beanData.buffer[8], data[8]);
  EXPECT_EQ(beanData.buffer[9], data[9]);
}

TEST_F(BeanTestClass, Transfer_more_than_BEANBUFFSIZE)
{
  unsigned char data[] = {0b00010100, 0x92, 0x11, 0x11, 0x22, 0x33, 0x11, 0x11, 0x22, 0x33, 0x11, 0x11, 0x22, 0x33, 0x11, 0x11, 0x22, 0x33, 0b01111110, 0b01000000};
  BeanTransfer beanTransfer;
  initBeanTransfer(&beanTransfer, data, sizeof(data) / sizeof(unsigned char));
  beanData.recBeanState = BEAN_NO_TR;

  while (getNextData(beanTransfer))
    recBean(&beanData, beanTransfer.bean, beanTransfer.cnt);

  EXPECT_EQ(beanData.recBufferFull, 0);

  EXPECT_EQ(beanData.recBuffPos, BEANBUFFSIZE);
  EXPECT_EQ(beanData.recBeanState, BEAN_TR_ERR);
  EXPECT_EQ(beanData.recBit, 7);
}

TEST_F(BeanTestClass, Should_Not_accept_any_transfer_when_in_BEAN_TR_ERR)
{
  unsigned char data[] = {0b00010100, 0x92, 0x11, 0x11, 0b01111110, 0b01000000};
  BeanTransfer beanTransfer;
  initBeanTransfer(&beanTransfer, data, sizeof(data) / sizeof(unsigned char));
  beanData.recBeanState = BEAN_TR_ERR;

  while (getNextData(beanTransfer))
    recBean(&beanData, beanTransfer.bean, beanTransfer.cnt);

  EXPECT_EQ(beanData.intBuffer[0], 0);
  EXPECT_EQ(beanData.intBuffer[1], 0);
  EXPECT_EQ(beanData.intBuffer[2], 0);
  EXPECT_EQ(beanData.buffer[0], 0);
  EXPECT_EQ(beanData.buffer[1], 0);
  EXPECT_EQ(beanData.buffer[2], 0);
  EXPECT_EQ(beanData.recBeanState, BEAN_TR_ERR);
  EXPECT_EQ(beanData.recBuffPos, 0);
  EXPECT_EQ(beanData.recBit, 7);
}

TEST_F(BeanTestClass, Should_Set_BEAN_NO_TR_immedeately_after_receiving_RSP)
{
  unsigned char data[] = {0x03, 0xAB, 0x12, 0x42, 0b01111110, 0b01000000};
  BeanTransfer beanTransfer;
  initBeanTransfer(&beanTransfer, data, sizeof(data) / sizeof(unsigned char));
  beanData.recBeanState = BEAN_NO_TR;

  while (getNextData(beanTransfer))
    recBean(&beanData, beanTransfer.bean, beanTransfer.cnt);

  // Buffer was rotated.
  EXPECT_EQ(beanData.buffer[0], data[0]);
  EXPECT_EQ(beanData.buffer[1], data[1]);
  EXPECT_EQ(beanData.buffer[2], data[2]);
  EXPECT_EQ(beanData.buffer[3], data[3]);
  EXPECT_EQ(beanData.buffer[4], data[4]);
  EXPECT_EQ(beanData.buffer[5], data[5]);

  EXPECT_EQ(beanData.recBufferFull, 1);
  EXPECT_EQ(beanData.recBeanState, BEAN_NO_TR);
}

TEST_F(BeanTestClass, Should_NOT_Set_BEAN_NO_TR_if_EOM_is_not_received_or_corrupted)
{
  // data contains incorrect EOM
  unsigned char data[] = {0x03, 0xAB, 0x12, 0x42, 0b01110110, 0b01000000};
  BeanTransfer beanTransfer;
  initBeanTransfer(&beanTransfer, data, sizeof(data) / sizeof(unsigned char));
  beanData.recBeanState = BEAN_NO_TR;

  while (getNextData(beanTransfer))
    recBean(&beanData, beanTransfer.bean, beanTransfer.cnt);

  // Buffer was not rotated as receive still in progress
  EXPECT_EQ(beanData.intBuffer[0], data[0]);
  EXPECT_EQ(beanData.intBuffer[1], data[1]);
  EXPECT_EQ(beanData.intBuffer[2], data[2]);
  EXPECT_EQ(beanData.intBuffer[3], data[3]);
  EXPECT_EQ(beanData.intBuffer[4], data[4]);
  EXPECT_EQ(beanData.intBuffer[5], data[5]);

  EXPECT_EQ(beanData.recBufferFull, 0);
  EXPECT_EQ(beanData.recBeanState, BEAN_TR_IN_PR);
}

TEST_F(BeanTestClass, getCntFromTmr_test)
{
  unsigned char t_cnt = 125;
  EXPECT_EQ(getCntFromTmr((t_cnt * 4) + 94, t_cnt), 5);
  EXPECT_EQ(getCntFromTmr((t_cnt * 4) + 92, t_cnt), 4);
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}