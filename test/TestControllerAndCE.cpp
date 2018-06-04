/**
 *  @file    TestCE.cpp
 *  @author  Thierry Pouplier (gortium)
 *  @date    15/02/2018
 *  @version 1.0
 *
 *  @brief Convolution element module Tests
 *
 *  @section DESCRIPTION
 *
 *  This file use googletest to test the CE module
 */


#include "fi/Fixed.hpp"
#include "fi/overflow/Saturate.hpp"
#include "fi/rounding/Classic.hpp"
#include "CNNPU/CE.hpp"
#include "CNNPU/Controller.hpp"
#include "CNNPU/HyperParams.hpp"
#include "gtest/gtest.h"
#include <queue>
#include <vector>
#include <deque>
//#include <CNNPU/Controller.hpp>

typedef Fi::Fixed<8,4,Fi::SIGNED,Fi::Throw,Fi::Classic> TestType;

/// Tests fixtures
struct CtrlrCEFixture : public ::testing::Test
{
protected:
  CtrlrFixture() {}
  virtual ~CtrlrFixture() {}
  CE<TestType>* _CE;
  CNNConfig cnnConfig;
  Controller<TestType>* _controller;
  void SetUp(const ConvLayerHParam convLayerHParam)
  {
    cnnConfig.push_back(convLayerHParam);
    Controller::CtrlrConsts ctrlrConsts(cnnConfig);
    _controller = new Controller<TestType>(ctrlrConsts);
    _CE = new CE<TestType>(convLayerHParam.filterSize, convLayerHParam.inputWidth + convLayerHParam.padding * 2);
  }
  virtual void TearDown()
  {
    delete _controller;
    delete _CE;
  }
};

/// Test data structures
struct ConvData
{
  std::vector< std::vector<TestType> > weights;
  std::vector<TestType> inputs;
  std::vector<TestType> results;
  TestType bias;
  ConvLayerHParam convLayerHParam;

  friend std::ostream&operator<<(std::ostream& os, const ConvData& obj)
  {
    os << "weight:";
    for(int i = 0 ; i < obj.weights.size(); ++i)
      for(int j = 0 ; j < obj.weights[i].size(); ++j)
        os << obj.weights[i][j] << "\n";
    return os;
  }
};

/// Test cases
struct ConvTestCase : CtrlrCEFixture, testing::WithParamInterface<ConvData> {};


/// The tests
TEST_P(ConvTestCase, ConvTest)
{
  Controller::CtrlSigs ctrlSigs;
  int resultIndex = 0, dataIndex = 0;

  // Get the data
  const ParamType data = GetParam();

  // Init CE
  SetUp(data.convLayerHParam);

  while(_controller->getOutSigs().stateBits != Controller::CtrlSigs::StateBits(0))
  {
    /// Input
    if (ctrlSigs.loadData)
    {
      _CE->setSigs(data.inputs[dataIndex], data.weights, data.bias);
      dataIndex++;
    }
    else
    {
      _CE->setSigs(TestType(0), data.weights, data.bias);
    }

    /// STEP
    _CE->step();
    _controller->step();

    /// Outputs
    _CE->getOutputSigs();
    ctrlSigs = _controller->getOutSigs();
    // Nothing good ever happen before that
    if (ctrlSigs.saveResult)
    {
      EXPECT_EQ(data.results[outHI][outWI], _CE->getOutputReg());
      resultIndex++;
    }
    else
    {
      // do nothing
    }
  }
}

/// Tests instantiations
INSTANTIATE_TEST_CASE_P(SmallInput, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector<TestType>{TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5)},

             std::vector<TestType>{TestType(2.75)},
             TestType(0.5),
             // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             ConvLayerHParam{3,3,1,1,3,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(BigInput, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector<TestType>{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},

             std::vector<TestType>{TestType(2.75),TestType(2.75),TestType(2.75),
                                   TestType(2.75),TestType(2.75),TestType(2.75),
                                   TestType(2.75),TestType(2.75),TestType(2.75)},
             TestType(0.5),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             ConvLayerHParam{5,5,1,1,3,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(SmallFilter, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5)}},

             std::vector<TestType>{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},

             std::vector<TestType>{TestType(1.5),TestType(1.5),TestType(1.5),TestType(1.5),
                                   TestType(1.5),TestType(1.5),TestType(1.5),TestType(1.5),
                                   TestType(1.5),TestType(1.5),TestType(1.5),TestType(1.5),
                                   TestType(1.5),TestType(1.5),TestType(1.5),TestType(1.5)},
             TestType(0.5),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             ConvLayerHParam{5,5,1,1,2,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(reallySmallFilter, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5)}},

             std::vector<TestType>{TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5)},

             std::vector<TestType>{TestType(0.25),TestType(0.25),TestType(0.25),
                                   TestType(0.25),TestType(0.25),TestType(0.25),
                                   TestType(0.25),TestType(0.25),TestType(0.25)},
             TestType(0.0),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             ConvLayerHParam{3,3,1,1,1,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(OnePadding, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector<TestType>{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},

             std::vector<TestType>{TestType(1.0),TestType(1.5), TestType(1.5), TestType(1.5), TestType(1.0),
                                   TestType(1.5),TestType(2.25),TestType(2.25),TestType(2.25),TestType(1.5),
                                   TestType(1.5),TestType(2.25),TestType(2.25),TestType(2.25),TestType(1.5),
                                   TestType(1.5),TestType(2.25),TestType(2.25),TestType(2.25),TestType(1.5),
                                   TestType(1.0),TestType(1.5), TestType(1.5), TestType(1.5), TestType(1.0)},
             TestType(0.0),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             ConvLayerHParam{5,5,1,1,3,1,1}
    }
));

INSTANTIATE_TEST_CASE_P(TwoPadding, ConvTestCase, testing::Values(
    // Weights, Inputs, Rsults, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector<TestType>{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},

             std::vector<TestType>{TestType(0.25), TestType(0.50), TestType(0.75), TestType(0.75), TestType(0.75), TestType(0.50), TestType(0.25),
                                   TestType(0.50), TestType(1.00), TestType(1.50), TestType(1.50), TestType(1.50), TestType(1.00), TestType(0.50),
                                   TestType(0.75), TestType(1.50), TestType(2.25), TestType(2.25), TestType(2.25), TestType(1.50), TestType(0.75),
                                   TestType(0.75), TestType(1.50), TestType(2.25), TestType(2.25), TestType(2.25), TestType(1.50), TestType(0.75),
                                   TestType(0.75), TestType(1.50), TestType(2.25), TestType(2.25), TestType(2.25), TestType(1.50), TestType(0.75),
                                   TestType(0.50), TestType(1.00), TestType(1.50), TestType(1.50), TestType(1.50), TestType(1.00), TestType(0.50),
                                   TestType(0.25), TestType(0.50), TestType(0.75), TestType(0.75), TestType(0.75), TestType(0.50), TestType(0.25)},
             TestType(0.0),                                                                                       
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             ConvLayerHParam{5,5,1,1,3,1,2}
    }
));

INSTANTIATE_TEST_CASE_P(TwoStride, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5)}},

             std::vector<TestType>{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),
                                   TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},

             std::vector<TestType>{TestType(1.5),TestType(1.5),TestType(1.5),
                                   TestType(1.5),TestType(1.5),TestType(1.5),
                                   TestType(1.5),TestType(1.5),TestType(1.5)},
             TestType(0.5),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             ConvLayerHParam{6,6,1,1,2,2,0}
    }
));

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}                                                                        