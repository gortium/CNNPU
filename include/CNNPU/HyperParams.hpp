//
// Created by gortium on 2/18/18.
//

#ifndef CNNPU_HYPERPARAMS_H
#define CNNPU_HYPERPARAMS_H

enum LayerType {UNDEFINED, CONVOLUTION, POOLING, FULL};

struct layerHParam
{
  const int inputWidth;
  const int inputHeight;
  const int inputDepth;
  const LayerType layerType;

  layerHParam(int inputWidth_p, int inputHeight_p, int inputDepth_p):
      inputWidth(inputWidth_p),
      inputHeight(inputHeight_p),
      inputDepth(inputDepth_p),
      layerType(UNDEFINED)
  {}
};

struct ConvLayerHParam : public layerHParam
{
  const int nbOfFilter;
  const int filterSize;
  const int stride;
  const int padding;

  ConvLayerHParam(int inputWidth_p, int inputHeight_p, int inputDepth_p, int nbOfFilter_p, int filterSize_p,
                  int stride_p, int padding_p):
      layerHParam(inputWidth_p, inputHeight_p, inputDepth_p),
      nbOfFilter(nbOfFilter_p),
      filterSize(filterSize_p),
      stride(stride_p),
      padding(padding_p),
      layerType(CONVOLUTION)
  {}
};

struct PoolLayerHParam : layerHParam
{
  const int filterSize;
  const int stride;

  PoolLayerHParam(int inputWidth_p, int inputHeight_p, int inputDepth_p, int filterSide_p, int stride_p):
      layerHParam(inputWidth_p, inputHeight_p, inputDepth_p),
      filterSize(filterSize_p),
      stride(stride_p),
      layerType(POOLING)
  {}
};

struct FullyConnectedHParam : ConvLayerHParam
{
  FullyConnectedHParam(int inputWidth_p, int inputHeight_p, int inputDepth_p, int nbOfFilter_p, int filterSize_p,
                       int stride_p, int padding_p):
      ConvLayerHParam(inputWidth_p, inputHeight_p, inputDepth_p, nbOfFilter_p, filterSize_p, stride_p, padding_p),
      layerType(FULL)
  {}
};

typedef std::vector<layerHParam> CNNConfig;

#endif //CNNPU_HYPERPARAMS_H
