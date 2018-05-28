//
// Created by gortium on 1/11/18.
//

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "Types.hpp"
#include <vector>

template <typename T>
class Memory
{
  private:

  bool _memEnableSig;
  bool _readWriteToggleSig;
  T _dataSig;
  T _dataReg;
  T _memAddressSig;

  /// A constant we're going to define which
  /// tells us the maximum address we can
  /// read or write from.
  const T _maxAddress;

  /// This is the memory space we're going to
  /// be using.
  std::vector<T> _memorySpace;

  public:

  /// Construct a Memory class instance
  /// for us, and clear the memory
  Memory(int memSize);

  /// Delete the memory class, releasing
  /// all the allocated memory space
  ~Memory();

  /// Function to clear the memory values all to zero
  void Clear();

  void setSigs(T memAddress, T data, bool memEnable, bool readWriteToggle);

  void step();

  T getOutputReg();

//  /// Function to read the given address value
//  const T &Read(const T &p_Address);
//
//  /// Function to write the value to the given address
//  void Write(const T &p_Address, const T &p_Value);

};


// --------------- Templatized Implementation ---------------

template<typename T>
Memory<T>::Memory(int memSize)
    :
    _maxAddress(memSize),  // The maximum constant
    _memEnableSig(false),
    _readWriteToggleSig(false),
    _dataSig(0),
    _memAddressSig(0)
{
  std::vector<T> _memorySpace(_maxAddress, 0);
  Clear(); // Our only function, clears the memory
  // in C++ we allocate the variables in the
  // constructor header, NOT here in the
  // Constructor body!
}

template<typename T>
Memory<T>::~Memory()
{}

template<typename T>
void Memory<T>::Clear()
{
  for (T i = 0; i < _maxAddress; ++i)
  {
    _memorySpace[i] = 0;
  }
}

template<typename T>
void Memory<T>::setSigs(T memAddress, T data, bool memEnable, bool readWriteToggle)
{
  _memAddressSig = memAddress;
  _dataSig = data;
  _memEnableSig = memEnable;
  _readWriteToggleSig = readWriteToggle;
}

template<typename T>
void Memory<T>::step()
{
  if(_memEnableSig)
  {
    if(_readWriteToggleSig)
    { // Read
      _dataReg = _memorySpace[_memAddressSig];
    }
    else
    { // write
      _memorySpace[_memAddressSig] = _dataReg;
    }
  }

  _dataReg = _dataSig;
}

template<typename T>
T Memory<T>::getOutputReg()
{
  return _dataReg;
}

//template<typename T>
//const T &Memory<T>::Read(const T &p_Address)
//{
//  return _memorySpace[p_Address];
//}
//
//template<typename T>
//void Memory<T>::Write(const T &p_Address, const T &p_Value)
//{
//  _memorySpace[p_Address] = p_Value;
//}

#endif //MEMORY_HPP
