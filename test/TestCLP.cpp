//
// Created by gortium on 2/15/18.
//


#include "../lib/libfi/include/fi/Fixed.hpp"
#include "../lib/libfi/include/fi/overflow/Saturate.hpp"
#include "../lib/libfi/include/fi/rounding/Classic.hpp"
#include "CLP/PE.hpp"
#include "gtest/gtest.h"

typedef Fi::Fixed<8,4,Fi::SIGNED,Fi::Saturate,Fi::Classic> TestType;

// Then we define a test fixture class template.
struct PETest : testing::Test
{
  PE<TestType>* _PE;

  PETest()
  {
    _PE = new PE<TestType>;
  }

  ~PETest()
  {
    delete _PE;
  }
};

struct MACOperation
{
  TestType _weight;
  TestType _input;
  TestType _carry;
  TestType _result;

  friend std::ostream&operator<<(std::ostream& os, const MACOperation& obj)
  {
    return os
        << "weight:" << obj._weight
        << "input:" << obj._input
        << "carry:" << obj._carry
        << "output:" << obj._result;
  }
};

struct MACOperationTest : PETest, testing::WithParamInterface<MACOperation>
{};

TEST_P(MACOperationTest, MACResult)
{
  TestType weight = GetParam()._weight;
  TestType input = GetParam()._input;
  TestType carry = GetParam()._carry;
  TestType result = GetParam()._result;

  _PE->step(input, weight, TestType(0));
  _PE->step(TestType(0), TestType(0), carry);
  _PE->step(TestType(0), TestType(0), TestType(0));

  TestType output = _PE->getReg2();
  EXPECT_EQ(result, output);
}

INSTANTIATE_TEST_CASE_P(Default, MACOperationTest, testing::Values(
      MACOperation{TestType(0),TestType(0),TestType(0),TestType(0)},
      MACOperation{TestType(1),TestType(1),TestType(0),TestType(1)},
      MACOperation{TestType(1),TestType(1),TestType(1),TestType(2)},
      MACOperation{TestType(1),TestType(2),TestType(1),TestType(3)},
      MACOperation{TestType(2),TestType(2),TestType(1),TestType(5)}
  ));


int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}