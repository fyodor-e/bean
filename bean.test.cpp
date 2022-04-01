#include <gtest/gtest.h>
#include "bean.X/bean.c"

// Instruction
// https://stackoverflow.com/questions/62910867/how-to-run-tests-and-debug-google-test-project-in-vs-code

typedef struct {
  unsigned char *pData;
  int dataSize;
  unsigned char byteTr;
  unsigned char bitTr;
  unsigned char bean;
  // staffing bit should be sent first
  unsigned char cnt;
} BeanTransfer;

class BeanTestClass : public ::testing::Test
{
protected:
  BeanData beanData;
  bool getNextData(BeanTransfer &beanTransfer)
  {
    // Dummy situation when we just starting transfer
    if (beanTransfer.byteTr == 0 && beanTransfer.bitTr == 0)
    {
      // We're starting transmission, set SOF
      beanTransfer.cnt = 1;
      beanTransfer.bean = 1;
      // Set up bit tr to first bit
      beanTransfer.bitTr = 7;
    }
    else
    {
      beanTransfer.cnt = 0;
      beanTransfer.bean = !beanTransfer.bean;
    }

    if (beanTransfer.byteTr < beanTransfer.dataSize)
    {
      while (beanTransfer.byteTr < beanTransfer.dataSize
        && !(beanTransfer.pData[beanTransfer.byteTr] & (0b1 << beanTransfer.bitTr)) == !beanTransfer.bean)
      {
        if (beanTransfer.bitTr == 0) {
          beanTransfer.bitTr = 7;
          beanTransfer.byteTr++;
        }
        else beanTransfer.bitTr--;
        beanTransfer.cnt++;
      }
    }
    return beanTransfer.byteTr < beanTransfer.dataSize;
  }

  virtual void SetUp()
  {
    memset(beanData.recBuffer[0], 0, BEANBUFFSIZE);
    memset(beanData.recBuffer[1], 0, BEANBUFFSIZE);
    memset(beanData.recBuffer[2], 0, BEANBUFFSIZE);
    beanData.pRecBuffer = beanData.recBuffer[0];
    beanData.recBit = 7;
    beanData.recBuffPos = 0;
    beanData.recBytesCount = 0;
    beanData.recBeanState = BEAN_NO_TR;
    beanData.recIsNextBitStaffing = 0;
  }
};

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
  unsigned char data[] = { 0b00010100, 0b10010010, 0b00010001, 0b00010001, 0b00100010, 0b00110011, 0b01111110, 0x01000000 };
  BeanTransfer beanTransfer;
  beanTransfer.pData = data;
  // byteTr and bitTr should be set to 0 to init transfer. See getNextData
  beanTransfer.byteTr = 0;
  beanTransfer.bitTr = 0;
  beanTransfer.bean = 0;
  // staffing bit should be sent first
  beanTransfer.cnt = 0;
  beanTransfer.dataSize = sizeof(data) / sizeof(unsigned char);
  beanData.recBeanState = BEAN_NO_TR;

  while(getNextData(beanTransfer)) recBean(&beanData, beanTransfer.bean, beanTransfer.cnt);

  EXPECT_EQ(beanData.pRecBuffer[0], data[0]);
  EXPECT_EQ(beanData.pRecBuffer[1], data[1]);
  EXPECT_EQ(beanData.pRecBuffer[2], data[2]);
  EXPECT_EQ(beanData.pRecBuffer[3], data[3]);
  EXPECT_EQ(beanData.pRecBuffer[4], data[4]);
  EXPECT_EQ(beanData.pRecBuffer[5], data[5]);
  EXPECT_EQ(beanData.pRecBuffer[6], data[6]);
  EXPECT_EQ(beanData.pRecBuffer[7], data[7]);
}


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}