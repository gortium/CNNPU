//
// Created by gortium on 1/15/18.
//

#ifndef CE_H
#define CE_H

#include "CLP/PE.hpp"
#include "CLP/Types.hpp"
#include <vector>
#include <queue>

/**
 * Convolutionnal Element
 * Objects that compute a convolution.
 *
 *@tparam T      Type of input and output data.
 *
 *                                                              bias
 *                                                                |
 *                                                               \/
 *             weigts, wEnable                     biasEnable-->[reg]
 *                      |                                         |
 *                     \/                                        \/
 *  [input row]--->[PE]--->[PE]--->[PE]---------------->[adder and reg]
 *       /\                                                       |
 *       |                                                       \/
 *  [input row]--->[PE]--->[PE]--->[PE]--->[syncRegs]--->[adder and reg]
 *       /\                                                       |
 *       |                                                       \/
 *  [input row]--->[PE]--->[PE]--->[PE]--->[syncRegs]--->[adder and reg]
 *       /\                                                       |
 *       |                                                       \/
 *     inputSig                                                    output
 */

template <typename T>
class CE
{
  private:
  int _size;
  T _bias;
  T _inputSig;
  T _output;
  bool _bEnable;
  bool _wEnable;
  std::vector<T> _adderRegs;
  std::vector< std::queue<T> > _syncRegs;

  std::vector< std::vector< PE<T> > > _PEs;
  std::vector< std::vector<T> > _weights;
  std::vector< std::queue<T> > _inputs;

  public:
  CE(int filterSize, int fifoSize);
  ~CE();
  int latency();
  void step();
  T getOutputReg();
  void setSigs(T input, std::vector< std::vector<T> > weights, bool wEnable, T bias, bool bEnable);
};

// --------------- Templatized Implementation ---------------

template<typename T>
CE<T>::CE(int filterSize, int fifoSize) :
    _size(filterSize),
    _bias(T(0)),
    _inputSig(T(0)),
    _output(T(0)),
    _adderRegs(_size, T(0)),
    _bEnable(0),
    _wEnable(0)
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

  _weights.resize(_size);
  for(int i=0; i < _weights.size(); i++)
  {
    _weights[i].assign(_size, T(0));
  }

  _inputs.resize(_size);
  for(int i=0; i < _inputs.size(); i++)
  {
    for(int j=0; j < fifoSize; j++)
    {
      _inputs[i].emplace(T(0));
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

template<typename T>
CE<T>::~CE()
{}

template<typename T>
int CE<T>::latency()
{
  int lag=0;

  // PE lag
  lag += (_PEs[0][0].latency())*_PEs.size()*2;
  return lag;
}

template<typename T>
T CE<T>::getOutputReg()
{
  return _output;
}

template <typename T>
void CE<T>::setSigs(T input, std::vector< std::vector<T> > weights, bool wEnable, T bias, bool bEnable)
{
  _inputSig = input;
  _bias = bias;
  _weights = weights;
  _wEnable = wEnable;
  _bEnable = bEnable;
}

template<typename T>
void CE<T>::step()
{
  // PE process
  // Example _size = 5. From 4 to 0
  for (int i = _size - 1; i >= 0; i--)
  {
    /// Row adders                                           TODO TODO TODO TODO TODO
    // For the first cycle, last adder
    if (i == _size - 1)
    {
      _output = _syncRegs[i - 1].front() + _adderRegs[i];
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
        _PEs[i][j].setSigs(_PEs[i][j - 1].getReg1(), _PEs[i][j - 1].getReg2(), _weights[i][j], _wEnable);
      }
      // For the last cycle, the first colum of PE
      else
      {
        _PEs[i][j].setSigs(_inputs[i].front(), T(0), _weights[i][j], _wEnable);
      }

      _PEs[i][j].step();
    }
  }

  /// Bias
  if (_bEnable)
  {
    _adderRegs.front() = _bias;
  }

  /// Inputs
  // new input into the lower fifo
  _inputs.back().push(_inputSig);
  // top of other fifo into the bottom of the next fifo
  for (int i = _inputs.size() - 1; i >= 1; i--)
  {
    _inputs[i - 1].push(_inputs[i].front());
    _inputs[i].pop();
  }
  _inputs.front().pop();
}

#endif //CE_H