#include <gtest/gtest.h>
#include "bean.X/sendBean.c"

// Instruction
// https://stackoverflow.com/questions/62910867/how-to-run-tests-and-debug-google-test-project-in-vs-code

class SendBeanTestClass : public ::testing::Test
{
protected:
  SendBeanData beanData;

  virtual void SetUp()
  {
    unsigned char data[] = { 0x06, 0b10010010, 0b00010001, 0b00010001, 0b00100010, 0b00110011, 0b01011010 };
    memcpy(beanData.sendBuffer, data, sizeof(data)/sizeof(data[0]));
    beanData.sentBit = 7;
    beanData.sendBuffPos = 0;
    // beanData.sendBytesCount = 0;
    beanData.cnt = 0;
    beanData.sendBeanState = BEAN_NO_TR;

    beanData.sendNextBitStaffing = 0;
    beanData.bean = 0;
  }
};

TEST_F(SendBeanTestClass, Should_send_bean_data)
{
  sendBean(&beanData);
  // SOF
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
  EXPECT_EQ(beanData.sendBeanState, BEAN_TR_SOF);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 0);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3); // With staffing
  EXPECT_EQ(beanData.bean, 1);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
  EXPECT_EQ(beanData.bean, 0);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
}
