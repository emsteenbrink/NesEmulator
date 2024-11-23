#include "Nes.h"

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

Nes::Nes(IPixelWindow& window)
  : m_cpu(m_cpuBus)
  , m_ppu(window, m_ppuBus, m_cpuBus)
{
  m_pRam = make_unique<RAM>((uint16_t)2048, (uint16_t)0x0000U, (uint16_t)0x1FFFU);
  m_cpuBus.AddComponent(*m_pRam);
  m_cpuBus.AddComponent(m_ppu);
  m_cpuBus.AddComponent(m_unusedRanges);
}

void Nes::AddController(Component& component)
{
  m_cpuBus.AddComponent(component);
}

void Nes::InsertCartridge(const std::filesystem::path& fileName)
{
  m_cartridge = make_shared<Cartridge>(m_cpuBus, m_ppuBus, fileName);
  m_cpu.Reset();
}

void Nes::Clock()
{
  m_ppu.Clock();
  if (m_clockCounter % 3 == 0)
  {
    bool ppuSteals = m_ppu.CPUClock(m_clockCounter);
    m_cpu.Clock(ppuSteals); // apu is also in here.
  }
  if (m_ppu.nmi)
  {
    m_ppu.nmi = false;
    m_cpu.NMI_Interrupt();
  }
  m_clockCounter++;
}

uint8_t Nes::ReadCpuBus(uint16_t address)
{
  uint8_t data = 0x00;
  m_cpuBus.Read(address, data);
  return data;
}

uint8_t Nes::ReadPpu(uint16_t address)
{
  return m_ppu.PpuBusReadDebug(address);
}
