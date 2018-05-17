//
// Created by gortium on 1/10/18.
//

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <iostream>       // std::cout
#include <string>         // std::string
#include "HyperParams.hpp"
#include "CE.hpp"



/**
 * Objects that control multiple (one for now) CE to perform convolution neural network computation.
 *
 *@tparam T      Type of input and output data.
 */
template <typename T>
class Controller
{
  private:
  T relu(T input);

  /// Indexes
  int _inWI, _inHI, _inDI, _outWI, _outHI, _outDI;
  int _layerSteps;
  int _state;
  /// Hyperparams
  LayerHParam _layerHParam;
  ///
  /// Buffers
  std::vector< std::vector< std::vector< std::vector<T> > > > _weights;
  std::vector< std::vector< std::vector<T> > > _inputs;
  std::vector<T> _bias;
  std::vector< std::vector< std::vector<T> > > _outputs;

  public:
  Controller(std::vector< CE<T> >& CEs, LayerHParam[]);
  void Step();
//  void LoadFromMem(Memory<T>& mem, T reg, int add);
//  void SaveToMem(Memory<T>& mem, T reg, int add);
};


// --------------- Templatized Implementation ---------------


template<typename T>
Controller<T>::Controller(std::vector< CE<T> >& CEs, LayerHParam layerHParam):
    /// Indexes
    _inWI(0), _inHI(0), _inDI(0), _outWI(0), _outHI(0), _outDI(0),
    _layerSteps(0),
    _state(1),
int maxStep =   data.layerHParam.inputHeight * data.layerHParam.inputWidth        /* Steps for all the image          */
                              + data.layerHParam.filterSize * 2                                 /* Steps between FIFO and output    */
                              + data.layerHParam.inputWidth + data.layerHParam.padding * 2 - 1  /* Step to get the last input out   */
                              + data.layerHParam.padding * data.layerHParam.inputWidth * 2      /* Steps for padding top and bottom */
                              + data.layerHParam.padding * data.layerHParam.inputHeight * 2     /* Steps for padding sides          */
                              + data.layerHParam.padding * data.layerHParam.padding * 4;        /* Steps for padding corners        */

int topPaddingSteps =   data.layerHParam.padding * data.layerHParam.inputWidth     // Top padding
                        + data.layerHParam.padding * data.layerHParam.padding * 2;  // Corner padding

int inputSteps =  data.layerHParam.inputHeight * data.layerHParam.inputWidth     // Inputs
                  + data.layerHParam.padding * data.layerHParam.inputHeight * 2  // Side padding
                  + data.layerHParam.padding * data.layerHParam.inputWidth       // Top padding
                  + data.layerHParam.padding * data.layerHParam.padding * 2;      // Corner padding

int outputSteps = data.layerHParam.filterSize * (data.layerHParam.inputWidth
                                                 + data.layerHParam.padding * 2)    // FIFO fill-up
                  + data.layerHParam.filterSize * 2  - 1                             // PE steps
                  + data.layerHParam.filterSize                                      // Adders steps
                  - 1;                                                               // Because start at 0

int nextRowSteps = (data.layerHParam.inputWidth          // Scrap next row steps
                    + data.layerHParam.padding * 2        // Scrap next row padding steps
                    + data.layerHParam.filterSize - 1)    // Scrap next row transition steps
                   * (data.layerHParam.stride - 1)       // ..for every stride > 1
                   - 1;                                  // -1 because it begin down counting next step


    /// Hyperparams
    _layerHParam(layerHParam),
    _convResult()
    {


      // TODO Init buffers
    }

template<typename T>
Controller<T>::~Controller()
{}


template<typename T>
void Controller<T>::step()
{
  switch (_state)
  {
    case 0:  /// Halt
      break;

    case 1: /// Load weight and bias
      /// Actions
    _CE->setSigs(TestType(0), _weights, true, _bias, true);

      /// Transitions
      _state = 2;
    break;

    case 2: /// Compute convolution
      /// Actions
      // Input signals
      if (_layerSteps < _layerHParam.inputHeight * _layerHParam.inputWidth)
      {
        _CE->setSigs(_inputs[inHI][inWI], _weights, false, _bias, false);

        // Increment input data indexes
        if (inWI == _layerHParam.inputWidth - 1)
        {
          inWI = 0;
          inHI++;
        } else
        {
          inWI++;
        }
      } else
      {
        _CE->setSigs(TestType(0), _weights, false, _bias, false);
      }

      // Outputs signals
      if (_layerSteps > _layerHParam.filterSize * _layerHParam.inputWidth
              + _layerHParam.inputWidth * 2)
      {
        EXPECT_EQ(_results[outHI][outWI], _CE->getOutputReg());

        // Increment output data indexes
        if (inWI == _layerHParam.inputWidth - 1)
        {
          outWI = 0;
          outHI++;
        } else
        {
          outWI++;
        }
      }

      // Increment layer step index
      _layerSteps++;

      /// Transitions
      if(_layerSteps < _layerHParam.filterSize * _layerHParam.inputWidth   // Steps to fill the inputs FIFOs
                       + _layerHParam.filterSize * 2                           // Steps between FIFO and output
                       + _layerHParam.inputWidth - 1)                          // Steps to get the last input out)
      {
        _state = 0; // TODO Next layer maybe..
      }
      break;
  }
}

#endif //CONTROLLER_HPP