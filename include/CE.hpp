//
// Created by gortium on 1/15/18.
//

#ifndef CE_H
#define CE_H

#include "PE.hpp"
#include "Types.hpp"

template <typename T>
class CE
{
  private:
  PE<T> _PEs[][];
  std::vector<T> _row_regs;
  std::deque<std::vector<T>> _inputs;
  int _nb_row, _nb_col;


  public:
  CE(int nb_row, int nb_col);
  std::vector<std::vector<T>> _weights;
  T step();
  T getOutputReg();
  void LoadWeights(std::vector<std::vector<T>> weights[][]);
  void LoadInputs(std::vector<T> inputs[]);
};

// --------------- Templatized Implementation ---------------

template<typename T>
CE<T>::CE(int nb_row, int nb_col) :
    _nb_row(nb_row),
    _nb_col(nb_col),
    _row_regs(nb_row+1, 0)
{
  for(int i = 0; i < _nb_row; i++)
  {
    for(int j = 0; j < _nb_col; j++)
    {
      _PEs[i][j] = new PE<T>();
    }
  }
  std::vector<std::vector<T>> _weights(_nb_row, std::vector<T>(_nb_col, 0));
}

template<typename T>
CE<T>::~CE()
{
  for(int i = 0; i < _nb_row; i++)
  {
    for(int j = 0; j < _nb_col; j++)
    {
      delete _PEs[i][j];
    }
  }
}

template<typename T>
T CE<T>::getOutputReg()
{
  return _row_regs.back();
}

template<typename T>
void CE<T>::LoadWeights(std::vector<std::vector<T>> weights)
{
  _weights = weights;
}

template<typename T>
void CE<T>::LoadInputs(std::vector<T> inputs[])
{
  _inputs.push(inputs);
  _inputs.pop;
}

template<typename T>
T CE<T>::step()
{
  // Example _nb_row = 5. From 4 to 0
  for(int i = _nb_row-1; i >= 0; i--)
  {
    /// Row adders
    regs[i+1] = _PEs[i][_nb_col-1].getReg2() + _row_regs[i];

    /// Column Mac
    // Example _nb_col = 5. From 4 to 0
    for(int j = _nb_col-1; j >= 0; j--)
    {
      int inputCounter = 0;
      if(j != 0)
      {
        _PEs[i][j].step(_PEs[i][j-1].getReg1(), _weights[i][j], _PEs[i][j - 1].getReg2());
      }
      else
      {
        _PEs[i][j].step(input[i], _weights[i][j], _input[i][inputCounter]);
      }
      inputCounter++;
    }
  }
}

#endif //CE_H
