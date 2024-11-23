#pragma once

#include "R6502.h"
#include "APU.h"
#include "Bus.h"

class CPU_2A03
{
public:
  CPU_2A03(Bus& bus);;
  
  void Clock(bool clockCyclesStolen);
  void Reset();
  void IRQ_Interrupt();
  void NMI_Interrupt();

  const R6502& GetCPU() const { return m_cpu; };

private:
  R6502 m_cpu;
  APU   m_apu;
};
