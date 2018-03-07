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
 *                                             bias
 *                                               |
 *                                              \/
 *             weigts, wEnable    biasEnable-->[reg]
 *                      |                        |
 *                     \/                       \/
 *  [ input row h ]--->[PE]--->[PE]--->[PE]--->[adder and reg]
 *       /\                                      |
 *       |                                      \/
 *  [input row h+1]--->[PE]--->[PE]--->[PE]--->[adder and reg]
 *       /\                                      |
 *       |                                      \/
 *  [input row h+2]--->[PE]--->[PE]--->[PE]--->[adder and reg]
 *       /\                                      |
 *       |                                      \/
 *     input                                   output
 */

template <typename T>
class CE
{
  private:
  std::vector< std::vector< PE<T> > > _PEs;
  std::vector< std::vector<T> > _weights;
  std::vector< std::queue<T> > _inputs;
  T _output;
  bool _bEnable, _wEnable;
  std::vector<T> _row_regs;
  int _size, _size;

  public:
  CE(int nb_row, int nb_col);
  ~CE();
  T step();
  T getOutputReg();
  void LoadWeights( std::vector< std::vector<T> > weights );
  void LoadWeights( T bias );
  void LoadInputs( T input );
};

// --------------- Templatized Implementation ---------------

template<typename T>
CE<T>::CE(int size) :
    _size(size),
    _size(nb_col),
    _bias(0),
    _row_regs(nb_row+1, 0),
    _bEnable(0),
    _wEnable(0)
{
  std::vector< std::vector< PE<T> > > _PEs(_size, std::vector< PE<T> >(new PE<T>()));
  std::vector< std::vector<T> > _weights(_size, std::vector<T>(_size, T(0)));
  std::vector< std::vector<T> > _inputs(_size, std::queue<T>());
  std::vector<T> _row_regs(T(0));
}

template<typename T>
CE<T>::~CE()
{}

template<typename T>
T CE<T>::getOutputReg()
{
  return _row_regs.back();
}

template <typename T>
T CE<T>::setSigs(T input, std::vector< std::vector<T> > weights, T wEnable, T bias, T bEnable)
{
  _intput = input;
  _bias = bias;
  _weights = weights;
  _wEnable = wEnable;
  _biasEnable = bEnable;
}

template<typename T>
T CE<T>::step()
{
  // PE process
  // Example _size = 5. From 4 to 0
  for(int i = _size-1; i >= 0; i--)
  {
    /// Row adders
    if(i == _size-1)
    {
      _row_regs[i+1] = _PEs[i][_size-1].getReg2() + _bias;
    }
    else if(i == 0)
    {
      _output = _PEs[i][_size-1].getReg2() + _row_regs[i];
    }
    else
    {
      _row_regs[i+1] = _PEs[i][_size-1].getReg2() + _row_regs[i];
    }

    /// Column Mac
    // Example _size = 5. From 4 to 0
    for(int j = _size-1; j >= 0; j--)
    {
      if(j != 0)
      {
        _PEs[i][j].setSigs(_PEs[i][j-1].getReg1(), _PEs[i][j - 1].getReg2(), weights[i][j], _wEnable);
        _PEs[i][j].step();
      }
      else
      {
        _PEs[i][j].setSigs(_inputs[i].front, T(0), weights[i][j], _wEnable);
        _PEs[i][j].step();
      }
    }
  }

  // bias signal to reg
  if(_bEnable)
  {
    _row_regs.front() = _bias;
  }

  // new input into the lower fifo
  _inputs.back().push(_input);
  // top of other fifo into the bottom of the next fifo
  for(int i = 0; i < _inputs.size(); i++)
  {
    _inputs[i + 1].push(_inputs[i].front());
    _inputs[i].pop();
  }
  _inputs.front().pop();
}

#endif //CE_H
