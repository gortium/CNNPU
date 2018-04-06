//
// Created by gortium on 2/15/18.
//


#include "../lib/libfi/include/fi/Fixed.hpp"
#include "../lib/libfi/include/fi/overflow/Saturate.hpp"
#include "../lib/libfi/include/fi/rounding/Classic.hpp"
#include "CLP/CE.hpp"
#include "CLP/PE.hpp"
#include "CLP/HyperParams.hpp"
#include "gtest/gtest.h"
#include <queue>
#include <vector>
#include <deque>

typedef Fi::Fixed<8,4,Fi::SIGNED,Fi::Throw,Fi::Classic> TestType;

/// Tests fixtures
struct CEFixture : public ::testing::Test
{
  protected:
  CE<TestType>* _CE;
  CEFixture() {}
  virtual ~CEFixture() {}
  void SetUp(const int filterSize, const int fifoSize)
  {
    _CE = new CE<TestType>(filterSize, fifoSize);
  }
  virtual void TearDown() {
    delete _CE;
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
  std::vector<TestType> zeroInput;

  // Variables
  int inWI = 0, inHI = 0, outWI = 0, outHI = 0;

  // Init CE
  SetUp(data.layerHParam.filterSize, data.layerHParam.inputWidth);

  /// Load weight and bias
  _CE->setSigs(TestType(0), data.weights, true, data.bias, true);
  _CE->step();
  EXPECT_EQ(TestType(0), _CE->getOutputReg());

  for(int i = 0;
      i < data.layerHParam.filterSize * data.layerHParam.inputWidth
          + data.layerHParam.inputWidth * 2
          + (data.layerHParam.inputWidth - 1);
      i++)
  {
    /// Input signals
    if(i < data.layerHParam.inputHeight * data.layerHParam.inputWidth)
    {
      _CE->setSigs(data.inputs[inHI][inWI], data.weights, false, data.bias, false);

      // Increment input data indexes
      if(inWI == data.layerHParam.inputWidth - 1)
      {
        inWI = 0;
        inHI++;
      }
      else
      {
        inWI++;
      }
    }
    else
    {
      _CE->setSigs(TestType(0), data.weights, false, data.bias, false);
    }

    /// STEP
    _CE->step();

    /// Outputs signals
    if(i > data.layerHParam.filterSize * data.layerHParam.inputWidth
            + data.layerHParam.inputWidth * 2)
    {
      EXPECT_EQ(data.results[outHI][outWI], _CE->getOutputReg());

      // Increment output data indexes
      if(inWI == data.layerHParam.inputWidth - 1)
      {
        outWI = 0;
        outHI++;
      }
      else
      {
        outWI++;
      }
    }
  }
}

/// Tests instantiations
INSTANTIATE_TEST_CASE_P(Default, ConvTestCase, testing::Values(
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
    },
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
    },
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(1.5),TestType(1.5),TestType(1.5)},
                                                  {TestType(1.5),TestType(1.5),TestType(1.5)},
                                                  {TestType(1.5),TestType(1.5),TestType(1.5)}},
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