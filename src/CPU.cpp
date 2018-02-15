//
// Created by gortium on 1/15/18.
//

// ----------------- CPU.cpp ----------------------

#include "../include/CPU.h"

#include <iostream>

namespace CPU_4001
{

  CPU::CPU(Memory* p_TheMemory)
      :
      c_ReservedAddress(0),
      c_BaseAddress(1),
      c_AddressCeiling(253),
      m_ProgramCounter(c_BaseAddress),
      m_Register0(0),
      m_Register1(0),
      m_OverflowError(false),
      m_UnderflowError(false),
      m_SignedMode(false),
      m_TheMemory(p_TheMemory),  // DOH!
      m_Halt(false)
  {
  }

  CPU::~CPU()
  {
    m_TheMemory = nullptr;
  }

  void CPU::Reset()
  {
    m_Halt = false;
    m_ProgramCounter = c_BaseAddress; // FIX!
    m_OverflowError = false;
    m_UnderflowError = false;
    m_SignedMode = false;
  }

  const byte CPU::Fetch()
  {
    byte l_opCode = 0;
    l_opCode = m_TheMemory->Read(m_ProgramCounter); // Whoops!
    ++m_ProgramCounter;
    if ( m_ProgramCounter > c_AddressCeiling )
    {
      Halt();
    }
    return l_opCode;
  }

  void CPU::Halt()
  {
    m_Halt = true;
  }

  void CPU::Add()
  {
    m_Register0 = m_Register0 + m_Register1;
  }

  void CPU::Beep()
  {
    std::cout << '\a';
  }

  void CPU::Store()
  {
    // Load the target address into register 1
    m_Register1 = m_TheMemory->Read(m_ProgramCounter);
    ++m_ProgramCounter; // Skip the memory location data
    // Write the register 0 value to this address
    m_TheMemory->Write(m_Register1, m_Register0);
    // Remember the order of our parameters
    // was ADDRESS then VALUE!
  }

  void CPU::Print()
  {
    // Load the target addressinto register 1 // DOH!
    m_Register1 = m_TheMemory->Read(m_ProgramCounter);
    ++m_ProgramCounter;

    // The value in the register is now the value to load
    m_Register0 = m_TheMemory->Read(m_Register1);

    // Output the register
    std::cout << (int)m_Register0 << std::endl;
    // I'm going to add endline!
  }

  void CPU::Decode(const byte& p_OpCode) // DOH!
  {
    // Special, when the system has pre-halted in Fetch
    // We do not decode!
    if ( m_Halt ) return;


    switch (p_OpCode)
    {
      // HALT
      case 0:
        Halt();
        break;

        // Load 0
      case 1:
        m_Register0 = m_TheMemory->Read(m_ProgramCounter);
        // Skip past the data!
        ++m_ProgramCounter;
        break;

        // Load 1
      case 2:
        m_Register1 = m_TheMemory->Read(m_ProgramCounter);
        // Skip past the data!
        ++m_ProgramCounter;
        break;

        // Add
      case 3:
        Add();
        break;

        // Beep
      case 4:
        Beep();
        break;

        // Store operation
      case 5:
        Store();
        break;

        // Print operation
      case 6:
        Print();
        break;
    }
  }

  void CPU::Run()
  {
    // While we don't see a HALT, keep going
    while (!m_Halt)
    {
      // Fetch into the reserved memory
      // location
      m_TheMemory->Write(c_ReservedAddress, Fetch());

      // Decode from the memory reserved address
      Decode(m_TheMemory->Read(c_ReservedAddress));
    }
  }

}