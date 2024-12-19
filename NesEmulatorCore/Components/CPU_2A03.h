#pragma once

#include "R6502.h"
#include "APU.h"
#include "Bus.h"

#include <optional>

class CPU_2A03
{
public:
  CPU_2A03(Bus& bus, ISoundSampleProcessor& soundSampleProcessor, std::shared_ptr<ICpuLogger> cpuLogger = nullptr);
  
  void Clock(bool clockCyclesStolen, uint16_t ppuX, uint16_t ppuY);
  void Reset(std::optional<uint16_t> programCounter = std::nullopt);
  void IRQ_Interrupt();
  void NMI_Interrupt();

  const R6502& GetCPU() const { return m_cpu; };

private:
  R6502 m_cpu;
  APU   m_apu;
};
