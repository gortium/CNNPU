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

class MACOperation
{
  public:
  MACOperation(double weight, double input, double carry, double result):
      _weight(TestType(weight)),
      _input(TestType(input)),
      _carry(TestType(carry)),
      _result(TestType(result))
      {}
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
      MACOperation{0,0,0,0},
      MACOperation{1,1,0,1},
      MACOperation{1,1,1,2},
      MACOperation{1,2,1,3},
      MACOperation{2,2,1,5}
  ));


int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}