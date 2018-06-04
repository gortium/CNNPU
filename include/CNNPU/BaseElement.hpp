//
// Created by gortium on 02/06/18.
//

#ifndef CNNPU_BASE_ELEMENT_HPP
#define CNNPU_BASE_ELEMENT_HPP

template <typename T>
class BaseElement
{
public:
  typedef struct OutSigs;
  typedef struct InSigs;

  virtual int latency() = 0;
  virtual void step() = 0;
  virtual OutSigs getOutSigs() = 0;
  virtual void setSigs(InSigs) = 0;
};

#endif //CNNPU_BASE_ELEMENT_HPP
