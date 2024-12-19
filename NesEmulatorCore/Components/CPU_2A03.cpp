#include "CPU_2A03.h"

CPU_2A03::CPU_2A03(Bus& bus, ISoundSampleProcessor& soundSampleProcessor, std::shared_ptr<ICpuLogger> cpuLogger)
: m_cpu(bus, false, cpuLogger)
, m_apu(bus, soundSampleProcessor)
{
  bus.AddComponent(m_apu);
}

void CPU_2A03::Clock(bool clockCyclesStolen, uint16_t ppuX, uint16_t ppuY)
{
  if (!clockCyclesStolen)
    m_cpu.Clock(ppuX, ppuY);
  m_apu.Clock();
}

void CPU_2A03::Reset(std::optional<uint16_t> programCounter)
{
  m_cpu.Reset(programCounter);
}

void CPU_2A03::IRQ_Interrupt()
{
  m_cpu.IRQ_Interrupt();
}

void CPU_2A03::NMI_Interrupt()
{
  m_cpu.NMI_Interrupt();
}
