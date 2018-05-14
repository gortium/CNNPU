//
// Created by gortium on 1/10/18.
//

#ifndef PE_H
#define PE_H

/**
 * Processing Element
 * Objects that compute a MAC.
 *
 *@tparam T      Type of input and output data.
 *
 *  Sig1--->[reg0]-->[reg1]
 *       \
 *        \    [w]<---Sig3
 *         \  /  |
 *         (X)    <---wEnable
 *          |
 *          |
 *  Sig2-->(+)
 *          |
 *          ------->[reg2]
 *
 **/
template <typename T>
class PE
{
  private:
  T _reg0, _reg1, _reg2, _w, _sig1, _sig2, _sig3;
  bool _wEnable;

  public:
  PE();
  ~PE();
  void setSigs(T sig1, T sig2, T sig3, bool wEnable);
  T getReg1();
  T getReg2();
  T step();
};

// --------------- Templatized Implementation ---------------

template<typename T>
PE<T>::PE():
_reg0(T(0)),
_reg1(T(0)),
_reg2(T(0)),
_w(T(0)),
_sig1(T(0)),
_sig2(T(0)),
_sig3(T(0)),
_wEnable(false)
{}

template<typename T>
PE<T>::~PE()
{}

template<typename T>
void PE<T>::setSigs(T sig1, T sig2,  T sig3, bool wEnable)
{
  _sig1 = sig1;
  _sig2 = sig2;
  _sig3 = sig3;
  _wEnable = wEnable;
}

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
T PE<T>::step()
{
  // Internal signal propagation
  _reg1 = _reg0;
  _reg0 = _sig1;
  _reg2 = (_w * _sig1) + _sig2;
  if(_wEnable){_w = _sig3;}
}

#endif //PE_H