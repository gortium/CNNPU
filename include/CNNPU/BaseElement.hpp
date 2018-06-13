//
// Created by gortium on 02/06/18.
//

#ifndef CNNPU_BASE_ELEMENT_HPP
#define CNNPU_BASE_ELEMENT_HPP

class BaseElement
{
public:
  struct OutSigs{};
  struct InSigs{};

  virtual int latency() = 0;
  virtual void step() = 0;
  virtual void connect(InSigs& inSigs, OutSigs& outSigs) = 0;
//  virtual OutSigs getOutSigs() = 0;
//  virtual void setSigs(InSigs& inSigs) = 0;
};

#endif //CNNPU_BASE_ELEMENT_HPP
