/**
 *  @file    PE.hpp
 *  @author  Thierry Pouplier (gortium)
 *  @date    10/01/2018
 *  @version 1.0
 *
 *  @brief Processing Element module
 *
 *  @section DESCRIPTION
 *
 *  This module, when given a weight, a input and a partial result, compute a MAC operation.
 *  The added reg0 delay the input to the next PE and permit the partials results to be added together.
 *
 *       dataInputSig--->[delayReg]-->[dataOutputReg]
 *                   \
 *                    \    [w]<---weightSig
 *                     \  /
 *                     (X)
 *                      |
 *                      |
 *  partialResultSig-->(+)
 *                      |
 *                      ------->[outputReg]
 */


#ifndef PE_H
#define PE_H

//#include "BaseElement.hpp"
//
//class BaseElement;

/**
 * @brief Processing Element
 * Objects that compute a MAC.
 *
 *@tparam T      Type of input and output data.
 **/
template <typename T>
class PE //: public BaseElement
{
  private:
    T _delayReg, _dataOutputReg, _outputReg; ///< The PE registers as T type
    T _w;                  ///< The weight register as T type
    T _dataInputSig, _partialResultSig, _weightSig; ///< The PE signals as T type

  public:
    PE();
    ~PE();
    int latency();
    T getDataOutputReg();
    T getOutputReg();
    void setSigs(T dataInputSig, T partialResultSig, T weightSig);
    void step();
};

// --------------- Templatized Implementation ---------------
/**  
* @brief  PE object constructor
*  
* @tparam T Type of input and output data 
*/
template<typename T>
PE<T>::PE():
_delayReg(T(0)),
_dataOutputReg(T(0)),
_outputReg(T(0)),
_w(T(0)),
_dataInputSig(T(0)),
_partialResultSig(T(0)),
_weightSig(T(0))
{}
/**  
* @brief  PE object destructor
*
* @tparam T Type of input and output data
*
*/
template<typename T>
PE<T>::~PE()
{}
/**  
* @brief  Function used to know the PE latency
*
* @tparam T Type of input and output data
*  
* @return the PE latency in step as a int
*/
template<typename T>
int PE<T>::latency()
{
  return 1;
}
/**  
* @brief  Function used get the register 1 of the PE
*
* @tparam T Type of input and output data
*  
* @return the PE register 1 value as a T type
*/
template<typename T>
T PE<T>::getDataOutputReg()
{
  return _dataOutputReg;
}
/**  
* @brief  Function used get the register 2 of the PE
*
* @tparam T Type of input and output data
*  
* @return the PE register 2 value as a T type
*/
template<typename T>
T PE<T>::getOutputReg()
{
  return _outputReg;
}
/**  
* @brief  Function used to set the input signals before each step
*
* @tparam T Type of input and output data
*
* @param  dataInputSig is the next input to enter the PE as a T type
* @param  partialResultSig is the partial result from the previous PE as a T type
* @param  weightSig is the weights that are written to the weights registers as a T type
**/
template<typename T>
void PE<T>::setSigs(T dataInputSig, T partialResultSig,  T weightSig)
{
  _dataInputSig = dataInputSig;
  _partialResultSig = partialResultSig;
  _weightSig = weightSig;
}
/**
* @brief Execute one step. Need to be called every step
*
* @tparam T Type of input and output data
*/
template<typename T>
void PE<T>::step()
{
  // Internal signal propagation
  _dataOutputReg = _delayReg;
  _delayReg = _dataInputSig;
  _outputReg = (_w * _dataInputSig) + _partialResultSig;
  _w = _weightSig;
}

#endif //PE_H