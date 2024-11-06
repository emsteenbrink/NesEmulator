#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <filesystem>

namespace fs = std::filesystem;

enum class CPU_PPU_TimingMode : uint8_t
{
  RP2C02 = 0,
  RP2C07 = 1,
  MultipleRegion = 2,
  UMC6527P = 3
};

enum class MIRROR
{
  HARDWARE,
  HORIZONTAL,
  VERTICAL,
  ONESCREEN_LO,
  ONESCREEN_HI,
};

class iNesConfiguration
{
public:
  iNesConfiguration(const fs::path& filePath);
  
  bool HasTrainerData() const;
  const std::vector<uint8_t>& GetTrainerData() const;
  const std::vector<uint8_t>& GetPrgRomData() const;
  std::vector<uint8_t>& GetChrRomData();

  uint8_t getPrgBanks() const;
  uint8_t getChrBanks() const;

  bool HasPersistentMemory() const;

  uint16_t GetMapperId() const;
  MIRROR GetMirror() const;
  

private:
  uint16_t              m_mapperId;
  bool                  m_hasTrainerData = false;
  bool                  m_hasPersistentMemory = false;
  std::vector<uint8_t>  m_trainerData;
  std::vector<uint8_t>  m_prgRomData;
  std::vector<uint8_t>  m_chrRomData;
  MIRROR                m_hwMirror;
  uint8_t               m_prgBanks;
  uint8_t               m_chrBanks;
};

