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

#ifndef CNNP_MPE_HPP
#define CNNP_MPE_HPP

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
    T 

public:
    MPE();
    ~MPE();
    int latency();
    T getOutputReg();
    void setSigs(T dataInputSig, T partialResultSig, T weightSig);
    T step();
};
#endif //CNNP_MPE_HPP
