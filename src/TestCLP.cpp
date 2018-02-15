#include <iostream>
#include "../include/Types.hpp"
#include "../include/PE.hpp"
#include "../include/CE.hpp"
#include "Memory.hpp"

int main()
{
  Memory<type4> inputMem();
  Memory<type4> weightMem();
  Memory<type4> hyperParamMem();
  Memory<type4> biasMem();
  Memory<type4> outputMem();

  ////////////////////////////////////////////

  std::cout << "Testing PE" << std::endl;

  PE<type4> PE1;

  type4 w(1), x(3.55555555), c(2.0);

  type4 test = PE1.step(w,x,c);

  std::cout << test << std::endl;

//////////////////////////////////////////////

  std::cout << "Testing CE" << std::endl;

  //// Hyper parameter
  // kernel depth, bias number
  // 111111, 6 bit for at least 64 (+1 implied)
  int const K=2;
  // Kernel width and height
  // 1111, 4 bit for at least 16 (+1 implied)
  int const F=3;
  // Sliding by.. to the right
  // 0, 1 bit for at least 2 (+1 implied)
  int const S=1;
  // Zero-padding around the input
  // P=(F-1)/2 For same size in and out
  // 000
  int const P=1;
  // Input width
  //
  int const W1=5;
  // Input height
  int const H1=5;
  // Input depth
  int const D1=3;

  CE<type4> CE1(3,3);

}