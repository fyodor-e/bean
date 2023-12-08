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
      : data{0x03, 0b10010010, 0b00010001, 0b00010001, 0b01011010}
  {
  }

  virtual void SetUp()
  {
    beanData.sendBeanState = BEAN_NO_TR;
    initSendBeanData(&beanData, (unsigned char *)&data);
  }
};

TEST_F(SendBeanTestClass, Should_reset_Send_Bean_data)
{
  resetSendBuffer(&beanData);
  EXPECT_EQ(beanData.sendBuffer[0], 0);

  EXPECT_EQ(beanData.sentBit, 7);
  EXPECT_EQ(beanData.sendBuffPos, 0);
  EXPECT_EQ(beanData.cnt, 0);
  EXPECT_EQ(beanData.sendBeanState, BEAN_NO_TR);

  EXPECT_EQ(beanData.sendNextBitStaffing, 0);
  EXPECT_EQ(beanData.bean, 0);
}

TEST_F(SendBeanTestClass, Should_init_Send_Bean_data)
{
  EXPECT_EQ(beanData.sendBuffer[0], data[0]);
  EXPECT_EQ(beanData.sendBuffer[1], data[1]);
  EXPECT_EQ(beanData.sendBuffer[2], data[2]);
  EXPECT_EQ(beanData.sendBuffer[3], data[3]);
  EXPECT_EQ(beanData.sendBuffer[4], data[4]);
}

TEST_F(SendBeanTestClass, Should_NOT_alter_Send_Bean_data_if_sendBeanState_is_not_BEAN_NO_TR)
{
  beanData.sendBeanState = BEAN_TR_SOF;
  beanData.sendBuffer[0] = 0xAA;
  EXPECT_EQ(initSendBeanData(&beanData, (unsigned char *)&data), 0);

  EXPECT_EQ(beanData.sendBuffer[0], 0xAA);
  EXPECT_EQ(beanData.sendBeanState, BEAN_TR_SOF);
}

TEST_F(SendBeanTestClass, Should_NOT_start_send_if_state_is_BEAN_NO_TR)
{
  beanData.sendBeanState = BEAN_NO_TR;
  beanData.sendBuffer[0] = 0xAA;
  sendBean(&beanData);

  EXPECT_EQ(beanData.cnt, 0);
  EXPECT_EQ(beanData.sendBeanState, BEAN_NO_TR);
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
  EXPECT_EQ(beanData.cnt, 1); // With staffing
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3);
  EXPECT_EQ(beanData.bean, 1);
  // First byte ends
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
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
  // Second byte ends
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 4);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
  // Third byte ends
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
  // Forth byte ends
    sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
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
  // Fifth byte ends (CRC8)
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 6);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
  EXPECT_EQ(beanData.bean, 0);
  // EOM ends
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 0);
  EXPECT_EQ(beanData.bean, 0);
  EXPECT_EQ(beanData.sendBeanState, BEAN_NO_TR);
}

TEST_F(SendBeanTestClass, Start_with_0xFD)
{
  unsigned char data[15] = { 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC };
  memcpy(beanData.sendBuffer, data, 15);

  sendBean(&beanData);
  // SOF
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  EXPECT_EQ(beanData.sendBeanState, BEAN_TR_SOF);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0); // staffing
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);  // staffing

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
}

TEST_F(SendBeanTestClass, Start_with_0xF3)
{
  unsigned char data[5] = { 0xF3, 0xFF, 0xFF, 0xFF, 0xCC };
  memcpy(beanData.sendBuffer, data, 5);

  sendBean(&beanData);
  // SOF
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  EXPECT_EQ(beanData.sendBeanState, BEAN_TR_SOF);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3); // with staffing
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);  // staffing

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 6);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 0);
  EXPECT_EQ(beanData.bean, 0);
  EXPECT_EQ(beanData.sendBeanState, BEAN_NO_TR);
}

TEST_F(SendBeanTestClass, End_with_1)
{
  unsigned char data[5] = { 0xF3, 0xFF, 0xFF, 0xFF, 0xCF };
  memcpy(beanData.sendBuffer, data, 5);

  sendBean(&beanData);
  // SOF
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  EXPECT_EQ(beanData.sendBeanState, BEAN_TR_SOF);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3); // with staffing
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);  // staffing

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);

  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 5);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 3);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 4);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 6);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 2);
  EXPECT_EQ(beanData.bean, 0);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 1);
  EXPECT_EQ(beanData.bean, 1);
  sendBean(&beanData);
  EXPECT_EQ(beanData.cnt, 0);
  EXPECT_EQ(beanData.bean, 0);
  EXPECT_EQ(beanData.sendBeanState, BEAN_NO_TR);
}

