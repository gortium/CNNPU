/**
 *  @file    MPE.hpp
 *  @author  Thierry Pouplier (gortium)
 *  @date    17/05/2018
 *  @version 1.0
 *
 *  @brief TODO
 *
 *  @section DESCRIPTION
 *
 *  TODO
 *
 */

#ifndef CNNPU_MPE_HPP
#define CNNPU_MPE_HPP

#include <vector>

/**
 * @brief Pooling Element
 * Objects that compute a max pooling operation.
 *
 *@tparam T      Type of input and output data.
 **/
template <typename T>
class MPE
{
private:
    int _size;
    T _outputReg;
    std::vector< std::vector<T> > _inputSigs;

public:
    MPE(int size);
    ~MPE();
    int latency();
    T getOutputReg();
    void setSigs(std::vector< std::vector<T> > inputSig);
    void step();
};
// --------------- Templatized Implementation ---------------
/**
* @brief  MPE object constructor
*
* @tparam T Type of input and output data
*/
template<typename T>
MPE<T>::MPE(int size):
        _size(size),
        _outputReg(T(0))
{
    _inputSigs.resize(_size);
    for(int i=0; i < _inputSigs.size(); i++)
    {
        _inputSigs[i].assign(_size, T(0));
    }
}
/**
* @brief  CE object destructor
*
* @tparam T Type of input and output data
*
*/
template<typename T>
MPE<T>::~MPE()
{}
/**
* @brief  Function used to know the MPE latency
*
* @tparam T Type of input and output data
*
* @return the PE latency in step as a int
*/
template<typename T>
int MPE<T>::latency()
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
T MPE<T>::getOutputReg()
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
void MPE<T>::setSigs(std::vector< std::vector<T> > inputSig)
{
    _inputSigs = inputSig;
}
/**
* @brief Execute one step. Need to be called every step
*
* @tparam T Type of input and output data
*/
template<typename T>
void MPE<T>::step()
{
    // Internal signal propagation
    for(int i = 0; i < _inputSigs.size(); i++)
    {
        for(int j = 0; i < _inputSigs[i].size(); i++)
        {
            _outputReg = _inputSigs[i][j] > _outputReg? _inputSigs[i][j] : _outputReg;
        }
    }
}

#endif //CNNPU_MPE_HPP
