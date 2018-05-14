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

  // Variables
  int inWI = 0, inHI = 0, outWI = 0, outHI = 0;
  int padCounter = data.layerHParam.padding;
  bool rowEndFlag = false;
  int scrapCounter = 0;
  bool scrapFlag = false;
  bool loadInputFlag = false;
  bool saveOutputFlag = false;


  int maxStep =   data.layerHParam.inputHeight * data.layerHParam.inputWidth        /* Steps for all the image          */
                + data.layerHParam.filterSize * 2                                 /* Steps between FIFO and output    */
                + data.layerHParam.inputWidth + data.layerHParam.padding * 2 - 1  /* Step to get the last input out   */
                + data.layerHParam.padding * data.layerHParam.inputWidth * 2      /* Steps for padding top and bottom */
                + data.layerHParam.padding * data.layerHParam.inputHeight * 2     /* Steps for padding sides          */
                + data.layerHParam.padding * data.layerHParam.padding * 4;        /* Steps for padding corners        */

  int topPaddingSteps =   data.layerHParam.padding * data.layerHParam.inputWidth     // Top padding
                        + data.layerHParam.padding * data.layerHParam.padding * 2;  // Corner padding

  int inputSteps =  data.layerHParam.inputHeight * data.layerHParam.inputWidth     // Inputs
                  + data.layerHParam.padding * data.layerHParam.inputHeight * 2  // Side padding
                  + data.layerHParam.padding * data.layerHParam.inputWidth       // Top padding
                  + data.layerHParam.padding * data.layerHParam.padding * 2;      // Corner padding

  int outputSteps = data.layerHParam.filterSize * (data.layerHParam.inputWidth
                                                   + data.layerHParam.padding * 2)    // FIFO fill-up
                    + data.layerHParam.filterSize * 2  - 1                             // PE steps
                    + data.layerHParam.filterSize                                      // Adders steps
                    - 1;                                                               // Because start at 0

  int nextRowSteps = (data.layerHParam.inputWidth          // Scrap next row steps
                      + data.layerHParam.padding * 2        // Scrap next row padding steps
                      + data.layerHParam.filterSize - 1)    // Scrap next row transition steps
                     * (data.layerHParam.stride - 1)       // ..for every stride > 1
                     - 1;                                  // -1 because it begin down counting next step

  // Init CE
  SetUp(data.layerHParam.filterSize, data.layerHParam.inputWidth + data.layerHParam.padding * 2);

  /// Load weight and bias
  _CE->setSigs(TestType(0), data.weights, true, data.bias, true);
  _CE->step();

  for(int i = 0; i < maxStep; i++)
  {
    /// Controller
    // Inputs logic
    if(i < inputSteps && i >= topPaddingSteps) // else load zeros
    {
      if(padCounter > 0 && !rowEndFlag)
      {
        padCounter--;
        loadInputFlag = false;
      }
      else if(padCounter < data.layerHParam.padding && rowEndFlag)
      {
        padCounter++;
        loadInputFlag = false;

        if(padCounter == data.layerHParam.padding){rowEndFlag = false;}
      }
      else
      {
        loadInputFlag = true;

        // Increment input data indexes
        if(inWI == data.layerHParam.inputWidth - 1)
        {
          inWI = 0;
          inHI++;
          rowEndFlag = true;
        }
        else
        {
          inWI++;
        }
      }
    }
    else
    {
      loadInputFlag = false;
    }

    // Output logic
    if(i >= outputSteps)
    {
      if(!scrapFlag) // The good stuff ;)
      {
        // Check answer
        saveOutputFlag = true;

        // Increment output data indexes and scrapFlag
        // Next row
        if (outWI == (data.layerHParam.inputWidth - data.layerHParam.filterSize + 2 * data.layerHParam.padding) /
                     data.layerHParam.stride) // (+1) for for real output size formula
        {
          outWI = 0;
          outHI++;
          // End of row logic
          if(data.layerHParam.filterSize - 1 > 0) // Scrap transition steps
          {
            scrapFlag = true;
            scrapCounter += data.layerHParam.filterSize - 1;
          }
          // Stride new row logic
          if(data.layerHParam.stride - 1 > 0)  // Scrap next row steps because stride > 1
          {
            scrapFlag = true;
            scrapCounter += nextRowSteps;
          }
        }
        else // Next column
        {
          outWI++;
          // Stride inside a row logic
          if(data.layerHParam.stride - 1 >= 1)
          {
            scrapFlag = true;
            scrapCounter = data.layerHParam.stride - 1;
          }
        }
      }
      else if(scrapFlag) // Scrap, dont take that output..
      {
        // Increment the scrap counter / flag
        saveOutputFlag = false;
        scrapCounter--;
        if(scrapCounter == 0){scrapFlag = false;}
      }
    }

    /// Input
    if(loadInputFlag)
    {
      _CE->setSigs(data.inputs[inHI][inWI], data.weights, false, data.bias, false);
    }
    else
    {
      _CE->setSigs(TestType(0), data.weights, false, data.bias, false);
    }

    /// STEP
    _CE->step();

    /// Outputs
    // Nothing good ever happen before that
    if(saveOutputFlag)
    {
      EXPECT_EQ(data.results[outHI][outWI], _CE->getOutputReg());
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

             std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(2.75)}},
             TestType(0.5),
             // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             LayerHParam{3,3,1,1,3,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(BigInput, ConvTestCase, testing::Values(
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
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             LayerHParam{5,5,1,1,3,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(SmallFilter, ConvTestCase, testing::Values(
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
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             LayerHParam{5,5,1,1,2,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(reallySmallFilter, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.25),TestType(0.25),TestType(0.25)},
                                                  {TestType(0.25),TestType(0.25),TestType(0.25)},
                                                  {TestType(0.25),TestType(0.25),TestType(0.25)}},
             TestType(0.0),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             LayerHParam{3,3,1,1,1,1,0}
    }
));

INSTANTIATE_TEST_CASE_P(OnePadding, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(1.0),TestType(1.5), TestType(1.5), TestType(1.5), TestType(1.0)},
                                                  {TestType(1.5),TestType(2.25),TestType(2.25),TestType(2.25),TestType(1.5)},
                                                  {TestType(1.5),TestType(2.25),TestType(2.25),TestType(2.25),TestType(1.5)},
                                                  {TestType(1.5),TestType(2.25),TestType(2.25),TestType(2.25),TestType(1.5)},
                                                  {TestType(1.0),TestType(1.5), TestType(1.5), TestType(1.5), TestType(1.0)}},
             TestType(0.0),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             LayerHParam{5,5,1,1,3,1,1}
    }
));

INSTANTIATE_TEST_CASE_P(TwoPadding, ConvTestCase, testing::Values(
    // Weights, Inputs, Rsults, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.25), TestType(0.5), TestType(0.75), TestType(0.75), TestType(0.75), TestType(0.5), TestType(0.25)},
                                                  {TestType(0.5),  TestType(1.0), TestType(1.5),  TestType(1.5),  TestType(1.5),  TestType(1.0), TestType(0.5)},
                                                  {TestType(0.75), TestType(1.5), TestType(2.25), TestType(2.25), TestType(2.25), TestType(1.5), TestType(0.75)},
                                                  {TestType(0.75), TestType(1.5), TestType(2.25), TestType(2.25), TestType(2.25), TestType(1.5), TestType(0.75)},
                                                  {TestType(0.75), TestType(1.5), TestType(2.25), TestType(2.25), TestType(2.25), TestType(1.5), TestType(0.75)},
                                                  {TestType(0.5),  TestType(1.0), TestType(1.5),  TestType(1.5),  TestType(1.5),  TestType(1.0), TestType(0.5)},
                                                  {TestType(0.25), TestType(0.5), TestType(0.75), TestType(0.75), TestType(0.75), TestType(0.5), TestType(0.25)}},
             TestType(0.0),                                                                                       
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             LayerHParam{5,5,1,1,3,1,2}
    }
));

INSTANTIATE_TEST_CASE_P(TwoStride, ConvTestCase, testing::Values(
    // Weights, Inputs, Results, Bias, Layer params
    ConvData{std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)},
                                                  {TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5),TestType(0.5)}},

             std::vector< std::vector<TestType> >{{TestType(1.5),TestType(1.5),TestType(1.5)},
                                                  {TestType(1.5),TestType(1.5),TestType(1.5)},
                                                  {TestType(1.5),TestType(1.5),TestType(1.5)}},
             TestType(0.5),
        // inputWidth, inputHeight, inputDepth, nbOfFilter, filterSize, stride, padding
             LayerHParam{6,6,1,1,2,2,0}
    }
));

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}                                                                        