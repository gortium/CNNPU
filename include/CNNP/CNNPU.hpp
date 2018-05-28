/**
 *  @file    CNNPU.hpp
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

#ifndef CNNP_CNNPU_H
#define CNNP_CNNPU_H

#include "CE.hpp"
#include "Controller.hpp"
#include "MPE.hpp"
#include "Memory.hpp"
#include "Types.hpp"

/**
 * @brief Convolution Neural Network Processor
 *
 * @tparam T Type of input and output data
 */
template <typename T>
class CNNPU
{
private:

public:
    CNNPU();
    ~CNNPU();
    void step();
    void setSigs();
};

// --------------- Templatized Implementation ---------------

template<typename T>
/**
* @brief  CE object constructor
*
* @tparam T Type of input and output data
*
* @param  filterSize is the filter size (height and width are equal) as a int
* @param  fifoSize is the FIFO queue size as a int. Should be equal to the input width
*/
CNNPU<T>::CNNPU()
{

}

#endif //CNNP_CNNP_H
