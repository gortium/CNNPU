//
// Created by gortium on 2/15/18.
//


#include "fi/Fixed.hpp"
#include "fi/overflow/Saturate.hpp"
#include "fi/rounding/Classic.hpp"
#include "CNNP/Controller.hpp"
#include "CNNP/CE.hpp"
#include "CNNP/PE.hpp"
#include "CNNP/HyperParams.hpp"
#include "gtest/gtest.h"
#include <queue>
#include <vector>
#include <deque>

typedef Fi::Fixed<8,4,Fi::SIGNED,Fi::Throw,Fi::Classic> TestType;

/// Tests fixtures
struct CtrlFixture : public ::testing::Test
{
  protected:
  CE<TestType>* _CE;
  Controller _controller;
  CEFixture() {}
  virtual ~CEFixture() {}
  void SetUp(const int filterSize, const int inputWidth)
  {
    _CE = new CE<TestType>(filterSize, inputWidth);
    _controller = new controller<TestType>();
  }
  virtual void TearDown() {
    delete _CE;
    delete _controller;
  }
};

/// Test data structures
struct ConvData
{
  std::vector< std::vector<TestType> > weights;
  std::vector< std::vector<TestType> > inputs;
  std::vector< std::vector<TestType> > results;
  TestType bias;
  LayerHParam layerHParam;

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
struct ConvTestCase : CEFixture, testing::WithParamInterface<ConvData> {};


/// The tests
TEST_P(ConvTestCase, ConvTest)
{
  // Get the data
  const ParamType data = GetParam();

  // Variables
  int inWI = 0, inHI = 0, outWI = 0, outHI = 0;

  // Init CE
  SetUp(data.layerHParam.filterSize, data.layerHParam.inputWidth);

  for(int i = 0;
      i < data.layerHParam.inputHeight * data.layerHParam.inputWidth     // Steps for all the image
          + data.layerHParam.filterSize * 2                              // Steps between FIFO and output
          + data.layerHParam.inputWidth - 1                              // Steps to get the last input out
          + 1;                                                           // Steps to load weights and bias
      i++)
  {
    c
  }
}

/// Tests instantiations
INSTANTIATE_TEST_CASE_P(SmallInput, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(2.75)}},
             TestType(0.5),
             // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, sliding, padding
             LayerHParam{3,3,1,1,3,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(BiggerInput, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(2.75),TestType(2.75),TestType(2.75)},
                                                  {TestType(2.75),TestType(2.75),TestType(2.75)},
                                                  {TestType(2.75),TestType(2.75),TestType(2.75)}},
             TestType(0.5),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, sliding, padding
             LayerHParam{5,5,1,1,3,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(SmallerFilter, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(1.5),TestType(1.5),TestType(1.5),TestType(1.5)},
                                                  {TestType(1.5),TestType(1.5),TestType(1.5),TestType(1.5)},
                                                  {TestType(1.5),TestType(1.5),TestType(1.5),TestType(1.5)},
                                                  {TestType(1.5),TestType(1.5),TestType(1.5),TestType(1.5)}},
             TestType(0.5),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, sliding, padding
             LayerHParam{5,5,1,1,2,1,0}
    }
));

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}