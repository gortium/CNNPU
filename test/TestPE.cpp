//
// Created by gortium on 2/15/18.
//


#include "../lib/libfi/include/fi/Fixed.hpp"
#include "../lib/libfi/include/fi/overflow/Saturate.hpp"
#include "../lib/libfi/include/fi/rounding/Classic.hpp"
#include "CLP/PE.hpp"
#include "CLP/HyperParams.hpp"
#include "gtest/gtest.h"
#include <queue>
#include <vector>
#include <deque>

typedef Fi::Fixed<8,4,Fi::SIGNED,Fi::Saturate,Fi::Classic> TestType;

/// Tests fixtures
struct PEFixture : testing::Test
{
  PE<TestType>* _PE;

  PEFixture()
  {
    _PE = new PE<TestType>;
  }

  ~PEFixture()
  {
    delete _PE;
  }
};

/// Test data structures
struct singleMACData
{
  TestType weight;
  TestType input;
  TestType carry;
  TestType result;

  friend std::ostream&operator<<(std::ostream& os, const singleMACData& obj)
  {
    return os
        << "weight:" << obj.weight
        << "input:" << obj.input
        << "carry:" << obj.carry
        << "output:" << obj.result;
  }
};

struct continousMACData
{
  TestType weight;

  TestType input1;
  TestType carry1;
  TestType result1;

  TestType input2;
  TestType carry2;
  TestType result2;

  TestType input3;
  TestType carry3;
  TestType result3;

  friend std::ostream&operator<<(std::ostream& os, const continousMACData& obj)
  {
    return os
        << "weight:" << obj.weight

        << "input1:" << obj.input1
        << "carry1:" << obj.carry1
        << "output1:" << obj.result1

        << "input2:" << obj.input2
        << "carry2:" << obj.carry2
        << "output2:" << obj.result2

        << "input3:" << obj.input3
        << "carry3:" << obj.carry3
        << "output3:" << obj.result3;
  }
};

/// Test cases
struct singleMACTestCase : PEFixture, testing::WithParamInterface<singleMACData> {};
struct continousMACTestCase : PEFixture, testing::WithParamInterface<continousMACData> {};

/// The tests
TEST_P(singleMACTestCase, singleMACTest)
{
  // Get the data
  const ParamType data = GetParam();

  // Load weights
  _PE->setSigs(TestType(0), TestType(0), data.weight, true);
  _PE->step();

  // Check output & Compute
  EXPECT_EQ(TestType(0), _PE->getReg2());
  _PE->setSigs(data.input, data.carry, TestType(0), false);
  _PE->step();
  EXPECT_EQ(data.result, _PE->getReg2());
}

TEST_P(continousMACTestCase, continousMACTest)
{
  // Get the data
  const ParamType data = GetParam();

  // Load weights
  _PE->setSigs(TestType(0), TestType(0), data.weight, true);
  _PE->step();

  // Check output & Compute
  EXPECT_EQ(TestType(0), _PE->getReg2());
  _PE->setSigs(data.input1, data.carry1, TestType(0), false);
  _PE->step();
  EXPECT_EQ(data.result1, _PE->getReg2());
  _PE->setSigs(data.input2, data.carry2, TestType(0), false);
  _PE->step();
  EXPECT_EQ(data.result2, _PE->getReg2());
  _PE->setSigs(data.input3, data.carry3, TestType(0), false);
  _PE->step();
  EXPECT_EQ(data.result3, _PE->getReg2());
}

/// Test instantiation
INSTANTIATE_TEST_CASE_P(Positif, singleMACTestCase, testing::Values(
    // Positif
    singleMACData{ TestType(0),   TestType(0),      TestType(0),    TestType(0)     },
    singleMACData{ TestType(1),   TestType(1),      TestType(0),    TestType(1)     },
    singleMACData{ TestType(1),   TestType(1),      TestType(1),    TestType(2)     },
    singleMACData{ TestType(1),   TestType(2),      TestType(1),    TestType(3)     },
    singleMACData{ TestType(2),   TestType(2),      TestType(2),    TestType(6)     }
));

INSTANTIATE_TEST_CASE_P(Negatif, singleMACTestCase, testing::Values(
    // Negatif
    singleMACData{ TestType(-0),   TestType(0),     TestType(0),    TestType(0)     },
    singleMACData{ TestType(-1),   TestType(1),     TestType(1),    TestType(0)     },
    singleMACData{ TestType(-2),   TestType(2),     TestType(-1),   TestType(-5)    },
    singleMACData{ TestType(-3),   TestType(-2),    TestType(-5),   TestType(1)     },
    singleMACData{ TestType(4),    TestType(-2),    TestType(4),    TestType(-4)    }
));

INSTANTIATE_TEST_CASE_P(Fraction, singleMACTestCase, testing::Values(
    // Fraction
    singleMACData{ TestType(0.0),  TestType(0.0),   TestType(0.0),  TestType(0.0)   },
    singleMACData{ TestType(1.1),  TestType(1.1),   TestType(0.01), TestType(1.22)  },
    singleMACData{ TestType(1.5),  TestType(-1.02), TestType(1.4),  TestType(-0.13) },
    singleMACData{ TestType(-1.3), TestType(2.2),   TestType(-1),   TestType(-3.86) },
    singleMACData{ TestType(2.0),  TestType(2.5),   TestType(-2),   TestType(3)     }
));

INSTANTIATE_TEST_CASE_P(Default, continousMACTestCase, testing::Values(
    continousMACData{ TestType(0),    TestType(0),     TestType(0),    TestType(0),
                      TestType(1.1),   TestType(0.01), TestType(0.01),
                      TestType(-1.02), TestType(1.4),  TestType(1.4)    },

    continousMACData{ TestType(-1.25), TestType(2.1875), TestType(-1),   TestType(-3.734375),
                      TestType(2.5),    TestType(-2),   TestType(-5.125),
                      TestType(-3.25),  TestType(-5),   TestType(-0.9375) }
));

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}