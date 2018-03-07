//
// Created by gortium on 2/18/18.
//

#ifndef CLP_HYPERPARAMS_H
#define CLP_HYPERPARAMS_H

struct LayerHParam
{
  int inputWidth;
  int inputHeight;
  int inputDepth;
  int nbOfFilter;
  int filterSize;
  int sliding;
  int padding;
};

#endif //CLP_HYPERPARAMS_H
