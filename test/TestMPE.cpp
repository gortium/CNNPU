/**
 *  @file    TestCE.cpp
 *  @author  Thierry Pouplier (gortium)
 *  @date    17/05/2018
 *  @version 1.0
 *
 *  @brief Max-Pooling element module Tests
 *
 *  @section DESCRIPTION
 *
 *  This file use googletest to test the MPE module
 */


#include "fi/Fixed.hpp"
#include "fi/overflow/Saturate.hpp"
#include "fi/rounding/Classic.hpp"
#include "CNNPU/MPE.hpp"
#include "CNNPU/HyperParams.hpp"
#include "gtest/gtest.h"
#include <queue>
#include <vector>
#include <deque>

typedef Fi::Fixed<8,4,Fi::SIGNED,Fi::Throw,Fi::Classic> TestType;

/// Tests fixtures
struct MPEFixture : testing::Test
{
  MPE<TestType>* _MPE;

  MPEFixture() {}
  void SetUp(const int size)
  {
    _MPE = new MPE<TestType>(size);
  }

  ~MPEFixture()
  {
    delete _MPE;
  }
};

/// Test data structures
struct singleMPData
{
  int size;
  std::vector< std::vector<TestType> > inputs;
  TestType result;

  friend std::ostream&operator<<(std::ostream& os, const singleMPData& obj)
  {
    return os
        << "output:" << obj.result;
  }
};

/// Test cases
struct singleMPTestCase : MPEFixture, testing::WithParamInterface<singleMPData> {};

/// The tests
TEST_P(singleMPTestCase, singleMPTest)
{
  // Get the data
  const ParamType data = GetParam();

  SetUp(data.size);

  // Check output & Compute
  _MPE->setSigs(data.inputs);
  _MPE->step();
  EXPECT_EQ(data.result, _MPE->getOutputReg());
}

/// Test instantiation
INSTANTIATE_TEST_CASE_P(Positif, singleMPTestCase, testing::Values(
    // Positif
    singleMPData{3,
      std::vector< std::vector<TestType> >{{TestType(12),TestType(10),TestType(1)},
                                           {TestType(2),TestType(-5),TestType(0.5)},
                                           {TestType(3),TestType(5.5),TestType(8)}},
                 
      TestType(12)}
));

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}