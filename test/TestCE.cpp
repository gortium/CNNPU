//
// Created by gortium on 2/15/18.
//


#include "../lib/libfi/include/fi/Fixed.hpp"
#include "../lib/libfi/include/fi/overflow/Saturate.hpp"
#include "../lib/libfi/include/fi/rounding/Classic.hpp"
#include "CLP/CE.hpp"
#include "CLP/HyperParams.hpp"
#include "gtest/gtest.h"
#include <queue>
#include <vector>
#include <deque>

typedef Fi::Fixed<8,4,Fi::SIGNED,Fi::Saturate,Fi::Classic> TestType;

/// Tests fixtures
struct CEFixture : public ::testing::Test
{
  protected:
  CE<TestType>* _CE;
  CEFixture() {}
  virtual ~CEFixture() {}
  void SetUp(const int size)
  {
    _CE = new CE<TestType>(size);
  }
  virtual void TearDown() {
    delete _CE;
  }
};

/// Test data structures
struct singleConvData
{
  std::vector< std::vector<TestType> > weights;
  std::vector< std::vector<TestType> > inputs;
  std::deque< std::vector<TestType> > results;
  T _bias;
  LayerHParam layerHParam;

  //TODO
//  friend std::ostream&operator<<(std::ostream& os, const singleConvData& obj)
//  {
//    return os
//        << "weight:" << obj._inputs
//        << "input:" << obj._input
//        << "carry:" << obj._carry
//        << "output:" << obj._result;
//  }
};

/// Test cases
//struct singleConvTestCase : CEFixture, testing::WithParamInterface<singleConvData> {};

/// The tests
TEST_P(singleConvTestCase, singleConvTest)
{
  // Get the data
  const ParamType data = GetParam();
  std::vector<TestType> zeroInput;

  // Init CE
  SetUp(data.layerHParam.filterSize);

  // Load weight and bias
  _CE->setSigs(data.inputs[i][j], data.weights, true, data.bias, true);
  _CE->step();
  EXPECT_EQ(TestType(0), _CE->getOutputReg());

  // Load first inputs
  for(int i = 0; i < data.layerHParam.filterSize * data.layerHParam.inputWidth; i++)
  {
    // Load data to the CE
    _CE->setSigs(data.inputs[i][j], data.weights, false, data.bias, false);

    _CE->step();

    EXPECT_EQ(TestType(0), _CE->getOutputReg());
  }
  // Compute & check outputs
  // For each input row
  for(int i = 0; i < data.layerHParam.inputHeight; i++)
  {
    // For each input column
    for(int j = 0; j < data.layerHParam.inputWidth; j++ )
    {
      // Load data to the CE
      _CE->setSigs(data.inputs[i][j], data.weights, false, data.bias, false);

      _CE->step();

      EXPECT_EQ(data._results[i-data.layerHParam.filterSize][j], _CE->getOutputReg()); //TODO
    }
  }
  // Then, get the last outputs
  for(int i = 0; i < data.layerHParam.filterSize; i++)
  {
    for(int j = 0; j < data.layerHParam.inputWidth; j++ )
    {
      // Load data to the CE
      _CE->setSigs(data.inputs[i][j], data.weights, false, data.bias, false);

      _CE->step();

      EXPECT_EQ(data.results[i][j], _CE->getOutputReg()); //TODO
    }
  }
  EXPECT_EQ(TestType(0), _CE->getOutputReg());
}

/// Tests instantiations
INSTANTIATE_TEST_CASE_P(Default, continousMACTestCase, testing::Values(
   continousMACData{TestType(0),    TestType(0),     TestType(0),    TestType(0),
                    TestType(1.1),  TestType(1.1),   TestType(0.01), TestType(1.22),
                    TestType(1.5),  TestType(-1.02), TestType(1.4),  TestType(-0.13) },

   continousMACData{TestType(-1.3), TestType(2.2),   TestType(-1),   TestType(-3.86),
                    TestType(2.0),  TestType(2.5),   TestType(-2),   TestType(3),
                    TestType(0),    TestType(0),     TestType(0),    TestType(0)     }
));

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}