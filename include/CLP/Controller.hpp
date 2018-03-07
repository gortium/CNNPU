//
// Created by gortium on 1/10/18.
//

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <iostream>       // std::cout
#include <string>         // std::string
#include "HyperParams.hpp"

#define FILTER_SIZE 9
#define BIT_WIDHT 8

/**
 * Objects that control multiple CE to perform convolution layer computation.
 *
 *@tparam T      Type of input and output data.
 */
template <typename T>
class Controller
{
  private:
  /// Memories
  Memory<T>* _inputMem;
  Memory<T>* _weightMem;
  Memory<T>* _hParamMem;
  Memory<T>* _biasMem;
  Memory<T>* _outputMem;
  /// Convolution elements
  CE<T> _CE;
  /// Addresses
  int _inputAdd;
  int _weightAdd;
  int _hParamAdd;
  int _biasAdd;
  int _outputAdd;
  /// Registry
  T _inputReg;
  T _weightReg;
  T _hParamReg;
  T _biasReg;
  T _outputReg;
  /// Hyperparams
  LayerHParam _layerHParam;
  /// Buffers
  std::vector<std::vector<T>> _weights[][];
  std::queue<std::vector<T>> _inputs[][];
  std::vector<T> _bias[];
  std::deque<std::vector<T>> _outputs[][];

  public:
  Controller();
  void LoadFromMem(Memory<T>& mem, T reg, int add);
  void SaveToMem(Memory<T>& mem, T reg, int add);
  T Step();
};


// --------------- Templatized Implementation ---------------


template<typename T>
Controller<T>::Controller(Memory<T>& inputMem,
                          Memory<T>& weightMem,
                          Memory<T>& hParamMem,
                          Memory<T>& biasMem,
                          Memory<T>& outputMem,
                          CE<T>& CE):
    /// Memories
    _inputMem(inputMem),
    _weightMem(weightMem),
    _hParamMem(hParamMem),
    _biasMem(biasMem),
    _outputMem(outputMem),
    /// Convolution elements
    _CE(CE),
    /// Addresses
    _inputAdd(0),
    _weightAdd(0),
    _hParamAdd(0),
    _biasAdd(0),
    _outputAdd(0),
    /// Registry
    _inputReg(0),
    _weightReg(0),
    _hParamReg(0),
    _biasReg(0),
    _outputReg(0),
    /// Hyperparams
    _layerHParam(),
    _convResult()
    {
      // TODO Init buffers
    }

template<typename T>
Controller<T>::~Controller()
{}

template<typename T>
void Controller<T>::LoadFromMem(Memory<T>& mem, T& reg, int& add)
{
  reg = mem.Read(add);
  add++;
}

template<typename T>
void Controller<T>::SaveToMem(Memory<T>& mem, T& reg, int& add)
{
  mem.Write(add,reg);
  add++;
}

template<typename T>
void Controller<T>::SaveToBuffer(T& buffer[], T& data[])
{
  buffer = data;
}

template<typename T>
T Controller<T>::step()
{
  switch(_state)
  {
    /// Load layer hyperparam
    case 1:
    {
      //this.LoadFromMem(_hParamMem, _hParamReg, _hParamAdd);

      // TEST TODO
      _layerHParam.filterSize = 3;
      _layerHParam.inputDepth = 3;
      _layerHParam.inputHeight = 5;
      _layerHParam.inputWidth = 5;
      _layerHParam.nbOfFilter = 3; //TODO To take into consideration
      _layerHParam.padding = 1;
      _layerHParam.sliding = 1; //TODO To take into consideration
      // TEST

      _state = 2; //TODO
      break;
    }
      /// Load weights
    case 2:
    {
      for(int i=0; i < _layerHParam.filterSize; i++)
      {
        for(int j=0; j < _layerHParam.filterSize; j++)
        {
          this.LoadFromMem(_weightMem, _weights[i][j], _weightAdd);
        }
      }
      _CE.LoadWeights(_weights);

      // Transition
      _state = 3; //TODO
      break;
    }
      /// Load bias
    case 3:
    {
      for(int i=0; i < _layerHParam.nbOfFilter; i++)
      {
        this.LoadFromMem(_biasMem, _bias[i], _biasAdd);
      }

      // Transition
      _state = 4; //TODO
      break;
    }
      /// Load inputs
    case 4:
    {
      for(int i=0; i < _layerHParam.inputHeight + _layerHParam.padding*2; i++)
      {
        for (int j = 0; j < _layerHParam.inputWidth + _layerHParam.padding*2; j++)
        {
          if(j < _layerHParam.padding ||
             j >= _layerHParam.inputWidth + _layerHParam.padding ||
             i < _layerHParam.padding ||
             i >= _layerHParam.inputHeight + _layerHParam.padding)
          {
            _inputs[i]->push((T)0);
          }
          else
          {
            this.LoadFromMem(_inputMem, _inputs[i][j], _inputAdd);
          }
        }
      }

      // Transition
      _state = 5; //TODO
      break;
    }
      /// Push input to CE
    case 4:
    {
      _CE.LoadInputs(_inputs.pop());
      _heightCounter++;

      // Transition
      if(_heightCounter > _layerHParam.inputHeight)
      {
        _state = 5; //TODO
      }
      break;
    }
      /// Compute
    case 6:
    {
      _CE.step();
      _outputs[_heightCounter]->push_front(_CE.getOutputReg()); // TODO LAST WORK
      _widthCounter++;

      // Transition
      if(_widthCounter > _layerHParam.inputWidth + _layerHParam.padding*2)
      {
        _state = 4; //TODO
      }
      break;
    }
      /// Add bias
    case 5:
      _outputs += _bias;

      // Transition
      _state = 6; //TODO
      break;
  }
}

#endif //CONTROLLER_HPP