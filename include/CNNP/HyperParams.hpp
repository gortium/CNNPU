//
// Created by gortium on 2/18/18.
//

#ifndef CNNP_HYPERPARAMS_H
#define CNNP_HYPERPARAMS_H

struct LayerHParam
{
  int inputWidth;
  int inputHeight;
  int inputDepth;
  int nbOfFilter;
  int filterSize;
  int stride;
  int padding;
};

#endif //CNNP_HYPERPARAMS_H
