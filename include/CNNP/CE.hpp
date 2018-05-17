/** 
 *  @file    CE.hpp
 *  @author  Thierry Pouplier (gortium)
 *  @date    15/01/2018  
 *  @version 1.0 
 *  
 *  @brief Convolution element module
 *
 *  @section DESCRIPTION
 *  
 *  This module, when given weights, bias and input, compute a convolution. The PE placement and
 *  it input FIFO permit a stride in the input matrix at each step.
 *
 *                                                             biasSig
 *                                                                |
 *                                                               \/
 *             weigtSigs, wEnable                     bEnable-->[reg]
 *                      |                                         |
 *                     \/                                        \/
 *  [inputRegs row]--->[PE]--->[PE]--->[PE]---------------->[adder and reg]
 *       /\                                                       |
 *       |                                                       \/
 *  [inputRegs row]--->[PE]--->[PE]--->[PE]--->[syncRegs]--->[adder and reg]
 *       /\                                                       |
 *       |                                                       \/
 *  [inputRegs row]--->[PE]--->[PE]--->[PE]--->[syncRegs]--->[adder and reg]
 *       /\                                                       |
 *       |                                                       \/
 *     inputSig                                                outputReg
 */

#ifndef CE_H
#define CE_H

#include "CNNP/PE.hpp"
#include <vector>
#include <queue>

/**
 * @brief Convolutionnal Element. Objects that compute a convolution
 *
 * @tparam T Type of input and output data
 */

template <typename T>
class CE
{
  private:
  // Parameter
  int _size;                                  ///< The size of the filter as int 
  // Input signals
  T _biasSig;                                 ///< The bias signal as a T type
  std::vector< std::vector<T> > _weightSigs;  ///< The weights signals as a vector of vector of T type
  T _inputSig;                                ///< The inputs signal as a T type
 // Registers
  T _outputReg;                               ///< The output register as a T type
  std::vector<T> _adderRegs;                  ///< The adder registers as a vector of T type
  std::vector< std::queue<T> > _syncRegs;     ///< The synchronization registers as a vector of queue of T type
  std::vector< std::vector<T> > _weightRegs;  ///< The weights registers as a vector of vector of T type 
  std::vector< std::queue<T> > _inputRegs;    ///< The inputs registers as a vector of queue of T type 
  // Submodule
  std::vector< std::vector< PE<T> > > _PEs;   ///< A vector of vector containning PE submodules

  public:
  CE(int filterSize, int fifoSize);
  ~CE();
  int latency();
  void step();
  T getOutputReg();
  void setSigs(T input, std::vector< std::vector<T> > weights, T bias);
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
CE<T>::CE(int filterSize, int fifoSize) :
    _size(filterSize),
    _biasSig(T(0)),
    _inputSig(T(0)),
    _outputReg(T(0)),
    _adderRegs(_size, T(0))
{
  if(_size == 0)
  {
    throw std::runtime_error("CE Size cannot be 0");
  }

  _PEs.resize(_size);
  if(_PEs.size() != _size)
  {
    throw std::logic_error("Size of _PEs != to _size");
  }
  for(int i=0; i < _size; i++)
  {
    for(int j=0; j < _size; j++)
    {
      _PEs.at(i).push_back(PE<T>());
    }
  }

  _weightRegs.resize(_size);
  for(int i=0; i < _weightRegs.size(); i++)
  {
    _weightRegs[i].assign(_size, T(0));
  }

  _inputRegs.resize(_size);
  for(int i=0; i < _inputRegs.size(); i++)
  {
    for(int j=0; j < fifoSize; j++)
    {
      _inputRegs[i].emplace(T(0));
    }
  }

  _syncRegs.resize(_size-1);
  for(int i=0; i < _syncRegs.size(); i++)
  {
    for(int j=0; j < i+1; j++)
    {
      _syncRegs[i].emplace(T(0));
    }
  }
}
/**  
* @brief  CE object destructor
*
* @tparam T Type of input and output data
*
*/  
template<typename T>
CE<T>::~CE()
{}
/**  
* @brief  Function used to know the CE latency
*
* @tparam T Type of input and output data
*  
* @return the CE latency in step as a int
*/  
template<typename T>
int CE<T>::latency()
{
  int lag=0;

  // PE lag
  lag += (_PEs[0][0].latency())*_PEs.size()*2;
  return lag;
}
/**  
* @brief  Function used get the output register of the CE
*
* @tparam T Type of input and output data
*  
* @return the CE output register as a T type
*/  
template<typename T>
T CE<T>::getOutputReg()
{
  return _outputReg;
}
/**  
* @brief  Function used to set the input signals at before each step
*
* @tparam T Type of input and output data
*
* @param  input is the next input to enter the FIFOs as a T type
* @param  weights is the weights that are written to the weights registers if wEnable is HIGH
* @param  wEnable is the control signal that ennable the weights to be written
* @param  bias is the bias that is written to the bias registers if bEnable is HIGH
* @param  bEnable is the control signal that ennable the bias to be written
*  
* @return the CE output register as a T type
*/  
template <typename T>
void CE<T>::setSigs(T input, std::vector< std::vector<T> > weights, T bias)
{
  _inputSig = input;
  _biasSig = bias;
  _weightSigs = weights;
}
/**  
* @brief Execute one step. Need to be called every step 
*
* @tparam T Type of input and output data
*/  
template<typename T>
void CE<T>::step()
{
  // PE process
  // Example _size = 5. From 4 to 0
  for (int i = _size - 1; i >= 0; i--)
  {
    /// Row adders
    // If there is only one PE
    if(_size - 1 == 0)
    {
      _outputReg = _PEs[i][_size - 1].getReg2() + _adderRegs[i];
    }
    // For the first cycle, last adder
    else if (i == _size - 1)
    {
      _outputReg = _syncRegs[i - 1].front() + _adderRegs[i];
    }
      // For the last cycle, first adder
    else if (i == 0)
    {
      _adderRegs[i + 1] = _PEs[i][_size - 1].getReg2() + _adderRegs[i];
    }
      // All the others in the middle
    else
    {
      _adderRegs[i + 1] = _syncRegs[i - 1].front() + _adderRegs[i];
    }

    /// Sycn Registery
    if(i != 0)
    {
      _syncRegs[i - 1].push(_PEs[i][_size - 1].getReg2());
      _syncRegs[i - 1].pop();
    }

    /// Column Mac
    // Example _size = 5. From 4 to 0
    for(int j = _size - 1; j >= 0; j--)
    {
      // Every cycle exept the last one, first PE
      if (j != 0)
      {
        _PEs[i][j].setSigs(_PEs[i][j - 1].getReg1(), _PEs[i][j - 1].getReg2(), _weightRegs[i][j], _wEnableSig);
      }
      // For the last cycle, the first colum of PE
      else
      {
        _PEs[i][j].setSigs(_inputRegs[i].front(), T(0), _weightRegs[i][j], _wEnableSig);
      }

      _PEs[i][j].step();
    }
  }

  /// Bias
  _adderRegs.front() = _biasSig;

  /// Weights
  _weightRegs = _weightSigs;

  /// Inputs
  // new input into the lower fifo
  _inputRegs.back().push(_inputSig);
  // top of other fifo into the bottom of the next fifo
  for (int i = _inputRegs.size() - 1; i >= 1; i--)
  {
    _inputRegs[i - 1].push(_inputRegs[i].front());
    _inputRegs[i].pop();
  }
  _inputRegs.front().pop();
}

#endif //CE_H