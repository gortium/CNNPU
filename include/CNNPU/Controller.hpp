/**
 *  @file    Controller.hpp
 *  @author  Thierry Pouplier (gortium)
 *  @date    10/01/2018
 *  @version 1.0
 *
 *  @brief Controller of the CNNPU
 *
 *  @section DESCRIPTION
 *
 *
 */

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <iostream>       // std::cout
#include <string>         // std::string
#include "HyperParams.hpp"
#include "CE.hpp"
#include "BaseElement.hpp"

class BaseElement;

/**
 * Objects that control multiple (one for now) CE to perform convolution neural network computation.
 *
 *@tparam T      Type of input and output data.
 */
template <typename T>
class Controller : public BaseElement
{
  public:
    typedef struct CtrlrConsts
    {
      const int MAX_STEPS;
      const int TOP_PAD_STEPS;
      const int IN_STEPS;
      const int OUT_STEPS;
      const int NEXT_ROW_STEPS;

      CtrlrConsts();
      CtrlrConsts(CNNConfig cnnConfig);
    }
    CtrlrConsts;

    typedef struct CtrlrOutSigs : OutSigs
    {
      bool loadBiasWeights;
      bool loadData;
      bool saveResult;

      CtrlrOutSigs();
      CtrlrOutSigs(bool loadBiasWeights_p, bool loadData_p, bool saveResult_p );
    }
    CtrlrOutSigs;

    Controller(CtrlrConsts ctrlrConsts);
    ~Controller();
    int latency();
    void connect(InSigs& inSigs, CtrlrOutSigs& ctrlrOutSigs_p);
    void step();
//    CtrlSigs getOutSigs();
//    void setSigs();

  private:
    enum State{IDLE, INIT, NORMAL};
    int _inWI, _inHI, _inDI, _outWI, _outHI, _outDI;
    CtrlrOutSigs* _ctrlrOutSigs;
    State _state;
    CtrlrConsts _ctrlrConsts;

    void idleState();
    void initState();
    void normalState();
};


/// --------------- Templatized Implementation ---------------

template<typename T>
Controller<T>::Controller(CtrlrConsts ctrlrConsts):
    /// Indexes
    _inWI(0), _inHI(0), _inDI(0), _outWI(0), _outHI(0), _outDI(0),
    _ctrlrConsts(ctrlrConsts)
{}

template<typename T>
Controller<T>::~Controller()
{}

template<typename T>
int Controller<T>::latency()
{
  return 1;
}

template<typename T>
void Controller<T>::connect(InSigs& inSigs, CtrlrOutSigs& ctrlrOutSigs_p)
{
  _ctrlrOutSigs = ctrlrOutSigs_p;
}

//template<typename T>
//void Controller::setSigs()
//{
// // TODO ?
//}
//
//template<typename T>
//CtrlSigs Controller<T>::getOutputSigs()
//{
//  return _ctrlSigs;
//}

template<typename T>
void Controller<T>::step()
{
  static int stepCounter = 0;
  switch (_state)
  {
    case IDLE:
      idleState();
      break;
    case INIT:
      initState();
      break;
    case NORMAL:
      normalState();
      break;
  }
  stepCounter++;
}

template<typename T>
void Controller<T>::idleState()
{
  _ctrlrOutSigs.loadBiasWeights = false;
  _ctrlrOutSigs.loadData = false;
  _ctrlrOutSigs.saveResult = false;
}

template<typename T>
void Controller<T>::initState()
{
  _ctrlrOutSigs.loadBiasWeights = true;
  _ctrlrOutSigs.loadData = false;
  _ctrlrOutSigs.saveResult = false;
  _state = NORMAL;
}

template<typename T>
void Controller<T>::normalState()
{
//  static int inWI = 0, inHI = 0, inDI = 0, outWI = 0, outHI = 0, outDI = 0;
//  static int padCounter = 0;
//  static bool rowEndFlag = false;
//  static int NstepCounter = 0;
//
//  // Inputs logic
//  if(i < ctrlrConsts.inputSteps && i >= ctrlrConsts.topPaddingSteps) // else load zeros
//  {
//    // padding
//    if(padCounter > 0 && !rowEndFlag)
//    {
//      padCounter--;
//      _ctrlSigs.loadData = false;
//    }
//    else if(padCounter < data.layerHParam.padding && rowEndFlag)
//    {
//      padCounter++;
//      _ctrlSigs.loadData = false;
//
//      if(padCounter == data.layerHParam.padding){rowEndFlag = false;}
//    }
//      // no padding
//    else
//    {
//      _ctrlSigs.loadData = true;
//
//      // Increment input data indexe
//      if(inWI == data.layerHParam.inputWidth - 1)
//      {
//        inWI = 0;
//        inHI++;
//        rowEndFlag = true;
//      }
//      else
//      {
//        //if(i != ctrlrConsts.topPaddingSteps)
//        inWI++;
//      }
//    }
//  }
//  else
//  {
//    _ctrlSigs.loadData = false;
//  }
//
//  // Output logic
//  if(i >= ctrlrConsts.outputSteps)
//  {
//    if(!scrapFlag) // The good stuff ;)
//    {
//      // Check answer
//      _ctrlSigs.saveOutput = true;
//
//      // Increment output data indexes and scrapFlag
//      // Next row
//      if (outWI == (data.layerHParam.inputWidth - data.layerHParam.filterSize + 2 * data.layerHParam.padding) /
//                   data.layerHParam.stride) // (+1) for for real output size formula
//      {
//        outWI = 0;
//        if(i != ctrlrConsts.outputSteps){outHI++;}
//        // End of row logic
//        if(data.layerHParam.filterSize - 1 > 0) // Scrap transition steps
//        {
//          scrapFlag = true;
//          scrapCounter += data.layerHParam.filterSize - 1;
//        }
//        // Stride new row logic
//        if(data.layerHParam.stride - 1 > 0)  // Scrap next row steps because stride > 1
//        {
//          scrapFlag = true;
//          scrapCounter += ctrlrConsts.nextRowSteps;
//        }
//      }
//      else // Next column
//      {
//        //if(i != ctrlrConsts.outputSteps)
//        outWI++;
//        // Stride inside a row logic
//        if(data.layerHParam.stride - 1 >= 1)
//        {
//          scrapFlag = true;
//          scrapCounter = data.layerHParam.stride - 1;
//        }
//      }
//    }
//    else // Scrap, dont take that..
//    {
//      // Increment the scrap counter / flag
//      _ctrlSigs.saveOutput = false;
//      scrapCounter--;
//      if(scrapCounter == 0){scrapFlag = false;}
//    }
//  }
//
//  NstepCounter++;
//  if(NstepCounter <= _ctrlrConsts.MAX_STEPS){_state = IDLE;}
}

template<typename T>
Controller<T>::CtrlrConsts::CtrlrConsts():
MAX_STEPS(0),
TOP_PAD_STEPS(0),
IN_STEPS(0),
OUT_STEPS(0),
NEXT_ROW_STEPS(0)
{}

template<typename T>
Controller<T>::CtrlrConsts::CtrlrConsts (CNNConfig& cnnConfig):

    ConvLayerHParam* convLayerHParam = (ConvLayerHParam*)(cnnConfig);

    MAX_STEPS (cnnConfig[0].inputHeight * cnnConfig[0].inputWidth        /* Steps for all the image          */
               + cnnConfig[0].filterSize * 2                            /* Steps between FIFO and output    */
               + cnnConfig[0].inputWidth + cnnConfig[0].padding * 2 - 1  /* Step to get the last input out   */
               + cnnConfig[0].padding * cnnConfig[0].inputWidth * 2      /* Steps for padding top and bottom */
               + cnnConfig[0].padding * cnnConfig[0].inputHeight * 2     /* Steps for padding sides          */
               + cnnConfig[0].padding * cnnConfig[0].padding * 4),        /* Steps for padding corners        */

    TOP_PAD_STEPS (cnnConfig[0].padding * cnnConfig[0].inputWidth     // Top padding
                   + cnnConfig[0].padding * cnnConfig[0].padding * 2),  // Corner padding

    IN_STEPS (cnnConfig[0].inputHeight * cnnConfig[0].inputWidth     // Inputs
              + cnnConfig[0].padding * cnnConfig[0].inputHeight * 2  // Side padding
              + cnnConfig[0].padding * cnnConfig[0].inputWidth       // Top padding
              + cnnConfig[0].padding * cnnConfig[0].padding * 2      // Corner padding
              - 1),

    OUT_STEPS (cnnConfig[0].filterSize * (cnnConfig[0].inputWidth + cnnConfig[0].padding * 2)    // FIFO fill-up
               + cnnConfig[0].filterSize * 2  - 1                             // PE steps
               + cnnConfig[0].filterSize                                      // Adders steps
               - 1),                                                               // Because start at 0

    NEXT_ROW_STEPS ((cnnConfig[0].inputWidth           // Scrap next row steps
                     + cnnConfig[0].padding * 2        // Scrap next row padding steps
                     + cnnConfig[0].filterSize - 1)    // Scrap next row transition steps
                    * (cnnConfig[0].stride - 1)       // ..for every stride > 1
                    - 1)                           // -1 because it begin down counting next step
{}

template<typename T>
Controller<T>::CtrlrOutSigs::CtrlrOutSigs():
    loadBiasWeights(false),
    loadData(false),
    saveResult(false)
{}

template<typename T>
Controller<T>::CtrlrOutSigs::CtrlrOutSigs(bool loadBiasWeights_p, bool loadData_p, bool saveResult_p):
    loadBiasWeights(loadBiasWeights_p),
    loadData(loadData_p),
    saveResult(saveResult_p)
{}
#endif //CONTROLLER_HPP