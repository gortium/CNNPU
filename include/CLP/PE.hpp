//
// Created by gortium on 1/10/18.
//

#ifndef PE_H
#define PE_H

/**
 * Objects that compute a MAC.
 *
 *@tparam T      Type of input and output data.
 *
 *  --->[reg0]-->[reg1]--->
 *   \
 *    \    [w]
 *     \  /
 *     (X)
 *      |
 *  -->(+)
 *      |
 *      ---->[reg2]--->
 *
 */
template <typename T>
class PE
{
  private:
    T _reg0, _reg1, _reg2;

  public:

    PE();

    ~PE();

    T getReg2();

    T getReg1();

    T step(T input, T weight, T carry);
};


// --------------- Templatized Implementation ---------------

template<typename T>
PE<T>::PE():
_reg0(0),
_reg1(0),
_reg2(0)
{}

template<typename T>
PE<T>::~PE()
{}

template<typename T>
T PE<T>::getReg1()
{
  return _reg1;
}

template<typename T>
T PE<T>::getReg2()
{
  return _reg2;
}

template<typename T>
T PE<T>::step(T input, T weight, T carry)
{
  static T addResult, mulResult;
  _reg1 = _reg0;
  _reg0 = input;
  _reg2 = addResult;
  addResult = mulResult + carry;
  mulResult = (weight * input);
}

#endif //PE_H