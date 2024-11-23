#pragma once

#include "Components/Bus.h"
#include "Components/CPU_2A03.h"
#include "Components/PPU_2C02.h"
#include "Components/RAM.h"
#include "Components/Cartridge.h"
#include "IPixelWindow.h"

#include <memory>
#include <filesystem>

// helper class to not make unsued adrees ranges fail in the bus.
class UnusedRangesComponent : public Component
{
public:
  UnusedRangesComponent() : Component(Range(0x4018, 0x401F)) {}

  // Inherited via Component
  virtual uint8_t Read(uint16_t /*address*/) override { return 0; }
  virtual void Write(uint16_t /*address*/, uint8_t /*data*/) override {}
};

class Nes
{
public:
  Nes(IPixelWindow& window);

  void AddController(Component& component);
  void InsertCartridge(const std::filesystem::path& fileName);

  const R6502& GetCpu() { return m_cpu.GetCPU(); };
  void Clock();

  uint8_t ReadCpuBus(uint16_t address);
  uint8_t ReadPpu(uint16_t address); // read directly from the PPU since it has internals that we might want to read as well.

private:
  Bus                         m_cpuBus;
  Bus                         m_ppuBus;

  CPU_2A03                    m_cpu;
  PPU_2C02                    m_ppu;
  std::unique_ptr<RAM>        m_pRam;
  std::shared_ptr<Cartridge>  m_cartridge;
  unsigned int                m_clockCounter = 0;
  UnusedRangesComponent       m_unusedRanges;
};