#include <gtest/gtest.h>
#include "bean.X/bean.c"

// Instruction
// https://stackoverflow.com/questions/62910867/how-to-run-tests-and-debug-google-test-project-in-vs-code



class BeanTestClass : public ::testing::Test
{
protected:
  BeanData beanData;
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
  beanData.recBeanState = BEAN_TR_DATA;
  recBean(&beanData, 1, BEAN_NO_TR_COND);
  EXPECT_EQ(beanData.recBeanState, BEAN_NO_TR);
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}