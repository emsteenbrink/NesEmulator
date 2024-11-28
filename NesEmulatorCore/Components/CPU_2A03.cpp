#include "CPU_2A03.h"

CPU_2A03::CPU_2A03(Bus& bus, ISoundSampleProcessor& soundSampleProcessor)
: m_cpu(bus, false)
, m_apu(bus, soundSampleProcessor)
{
  bus.AddComponent(m_apu);
}

void CPU_2A03::Clock(bool clockCyclesStolen)
{
  if(!clockCyclesStolen)
    m_cpu.Clock();
  m_apu.Clock();
}

void CPU_2A03::Reset()
{
  m_cpu.Reset();
}

void CPU_2A03::IRQ_Interrupt()
{
  m_cpu.IRQ_Interrupt();
}

void CPU_2A03::NMI_Interrupt()
{
  m_cpu.NMI_Interrupt();
}
