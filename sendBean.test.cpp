#include <gtest/gtest.h>
#include "bean.X/sendBean.c"

// Instruction
// https://stackoverflow.com/questions/62910867/how-to-run-tests-and-debug-google-test-project-in-vs-code

class SendBeanTestClass : public ::testing::Test
{
protected:
  SendBeanData beanData;
  unsigned char data[10];

  SendBeanTestClass()
  : data { 0x06, 0b10010010, 0b00010001, 0b00010001, 0b00100010, 0b00110011, 0b01011010 }
  {}

  virtual void SetUp()
  {
    initSendBeanData (&beanData, (unsigned char *)&data);
  }
};

TEST_F(SendBeanTestClass, Should_init_Send_Bean_data)
{
    EXPECT_EQ(beanData.sendBuffer[0], data[0]);
    EXPECT_EQ(beanData.sendBuffer[1], data[1]);
    EXPECT_EQ(beanData.sendBuffer[2], data[2]);
    EXPECT_EQ(beanData.sendBuffer[3], data[3]);
    EXPECT_EQ(beanData.sendBuffer[4], data[4]);
    EXPECT_EQ(beanData.sendBuffer[5], data[5]);
    EXPECT_EQ(beanData.sendBuffer[6], data[6]);

    EXPECT_EQ(beanData.sentBit, 7);
    EXPECT_EQ(beanData.sendBuffPos, 0);
    EXPECT_EQ(beanData.cnt, 0);
    EXPECT_EQ(beanData.sendBeanState, BEAN_NO_TR);

    EXPECT_EQ(beanData.sendNextBitStaffing, 0);
    EXPECT_EQ(beanData.bean, 0);

}

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
