//
// Created by gortium on 1/11/18.
//

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "Types.hpp"

template <typename T>
class Memory
{
  public:

  /// A constant we're going to define which
  /// tells us the maximum address we can
  /// read or write from.
  const T c_MaxAddress;

  private:

  /// This is the memory space we're going to
  /// be using.
  T *m_MemorySpace;

  public:

  /// Construct a Memory class instance
  /// for us, and clear the memory
  Memory();

  /// Delete the memory class, releasing
  /// all the allocated memory space
  ~Memory();

  /// Function to clear the memory values all to zero
  void Clear();

  /// Function to read the given address value
  const T &Read(const T &p_Address);

  /// Function to write the value to the given address
  void Write(const T &p_Address, const T &p_Value);

};


// --------------- Templatized Implementation ---------------

template<typename T>
Memory<T>::Memory()
    :
    c_MaxAddress(255),  // The maximum constant
    m_MemorySpace(new T[c_MaxAddress]) // The memory
{
  Clear(); // Our only function, clears the memory
  // in C++ we allocate the variables in the
  // constructor header, NOT here in the
  // Constructor body!
}

template<typename T>
Memory<T>::~Memory()
{
  if (m_MemorySpace != nullptr)
  {
    delete[] m_MemorySpace;
    m_MemorySpace = nullptr;
  }
}

template<typename T>
void Memory<T>::Clear()
{
  for (T i = 0; i < c_MaxAddress; ++i)
  {
    m_MemorySpace[i] = 0;
  }
}

template<typename T>
const T &Memory<T>::Read(const T &p_Address)
{
  return m_MemorySpace[p_Address];
}

template<typename T>
void Memory<T>::Write(const T &p_Address, const T &p_Value)
{
  m_MemorySpace[p_Address] = p_Value;
}

#endif //MEMORY_HPP
