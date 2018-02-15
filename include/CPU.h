//
// Created by gortium on 1/15/18.
//

// -------- CPU.h -----------
#ifndef CPU_PROCESSOR
#define CPU_PROCESSOR
#include "Memory.hpp"
class CPU {
  public:
  const byte c_ReservedAddress;   const byte c_BaseAddress;   const byte c_AddressCeiling;

  private:
  byte m_ProgramCounter;   byte m_Register0;   byte m_Register1;   bool m_OverflowError;   bool m_UnderflowError;   bool m_SignedMode;   bool m_Halt;
  Memory* m_TheMemory;
  const byte Fetch();
  void Decode(const byte& p_OpCode);
  void Halt();
  void Add();
  void Beep();
  void Store();
  void Print();

  public:
  CPU(Memory* p_TheMemory);
  ~CPU();
  void Reset();
  void Run();
};
#endif
