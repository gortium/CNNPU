/**
 *  @file    CNNP.hpp
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

#ifndef CNNP_CNNP_H
#define CNNP_CNNP_H

/**
 * @brief Convolution Neural Network Processor
 *
 * @tparam T Type of input and output data
 */

template <typename T>
class CNNP
{
private:

public:
    CNNP(int filterSize, int fifoSize);
    ~CNNP();
    void step();
    void setSigs();
};

#endif //CNNP_CNNP_H
