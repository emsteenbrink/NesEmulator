#include "iNesConfiguration.h"

#include <fstream>
#include <cassert>
#include <exception>
#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;

struct Header
{
  char constant[4];
  uint8_t prgRomChunks;
  uint8_t chrRomChunks;
  
  struct
  {
    uint8_t mirroring : 1;
    bool hasPersistentMemory : 1;
    bool hasTrainer : 1;
    bool hasHardwired4ScreenMode : 1;
    uint8_t mapperLow : 4;
  } flags6;


  struct
  {
    uint8_t consoleType : 2;
    uint8_t fileFormat : 2;
    uint8_t mapperHigh : 4;
  } flags7;

  union {
    uint8_t PrgRamSize; // Nes 1.0
    struct // Nes 2.0
    {
      uint8_t mapperExt : 4;
      uint8_t subMapper : 4;
    } nes20;
  } flags8;

  union
  {
    struct
    {
      uint8_t tvSystem : 1;
      uint8_t : 7;
    } nes10;
    struct // Nes2.0
    {
      uint8_t prgRomChunksMSB : 4;
      uint8_t chrRomChunksMSB : 4;
    } nes20;
  } flags9;

  union
  {
    struct // Nes 1.0
    {
      uint8_t tvSystem : 2;
      uint8_t : 2;
      bool prgRamNotPresent : 1;
      bool boardHasConflicts : 1;
      uint8_t : 2;
    } nes10;
    struct // Nes 2.0
    {
      uint8_t prgRamSizeShiftCount : 4;
      uint8_t prgNVRamSizeShiftCount : 4;
    } nes20;
  } flags10;
  struct // Nes 2.0
  {
    uint8_t chrRamSizeShiftCount : 4;
    uint8_t chrNVRamSizeShiftCount : 4;
  } flags11;

  struct // Nes 2.0
  {
    CPU_PPU_TimingMode timingMode : 2;
    uint8_t : 6;
  } flags12;

  struct  // Nes 2.0
  {
    uint8_t vs_PPUType_Or_ExtConsoleType : 4;
    uint8_t vs_HardwareType : 4;
  } flags13;

  struct // Nes 2.0
  {
    uint8_t noMiscROMSPresent : 2;
    uint8_t : 6;
  } flags14;

  struct // Nes 2.0
  {
    uint8_t defaultExpansionDevice : 6;
    uint8_t : 2;
  } flags15;
};

iNesConfiguration::iNesConfiguration(const std::filesystem::path& filePath)
{
  ifstream file;
  file.open("/home/ernst/Data/Roms/Super_mario_brothers.nes", ios::binary);
  
  if (!file.is_open())
  {
    stringstream ss;
    ss << "File " << filePath << " not open. " << std::strerror(errno);
    throw std::runtime_error(ss.str());
  }

  Header header = { 0 };
  
  static_assert(sizeof(Header) == 16, "packing fails");

  file.read((char*)&header, sizeof(header));

  assert(header.constant[0] == 'N' && header.constant[1] == 'E' && header.constant[2] == 'S' && header.constant[3] == 0x1A);


  bool isNes2 = header.flags7.fileFormat == 2;

  m_mapperId = header.flags7.mapperHigh << 4 | header.flags6.mapperLow;
  if (isNes2)
    m_mapperId |= (uint16_t)header.flags8.nes20.mapperExt << 8;

  if (header.flags6.hasTrainer)
  { 
    m_hasTrainerData = true;
    m_trainerData.resize(512, 0x00);
    file.read((char*)m_trainerData.data(), 512);
  }

  m_hasPersistentMemory = header.flags6.hasPersistentMemory;

  uint32_t prgRomDataSize = 0;
  uint32_t chrRomDataSize = 0;

  if(isNes2)
  {
    if (header.flags9.nes20.prgRomChunksMSB == 0xf)
      throw std::runtime_error("Prg Rom extended/multiplier not supported");

    m_prgBanks = (uint16_t)header.flags9.nes20.prgRomChunksMSB << 8 | header.prgRomChunks;
    prgRomDataSize = m_prgBanks * 16384;

    if (header.flags9.nes20.chrRomChunksMSB == 0xf)
      throw std::runtime_error("Chr Rom extended/multiplier not supported");

    m_chrBanks = (uint16_t)header.flags9.nes20.chrRomChunksMSB << 8 | header.chrRomChunks;
    chrRomDataSize = m_chrBanks * 8192;
  }
  else
  {
    m_prgBanks = header.prgRomChunks;
    prgRomDataSize = header.prgRomChunks * 16384;

    m_chrBanks = header.chrRomChunks;
    if (m_chrBanks == 0)
    {
      chrRomDataSize = 8192;
    }
    else
    {
      chrRomDataSize = m_chrBanks * 8192;
    }
  }

  m_prgRomData.resize(prgRomDataSize);
  file.read((char*)m_prgRomData.data(), prgRomDataSize);
  
  m_chrRomData.resize(chrRomDataSize);
  file.read((char*)m_chrRomData.data(), chrRomDataSize);

  m_hwMirror = header.flags6.mirroring ? MIRROR::VERTICAL : MIRROR::HORIZONTAL;

  unsigned int count = 0;
  uint8_t rest = 0;
  while (!file.eof())
  {
    file.read((char*)&rest, 1);
    count++;
  }
  //assert(count == 0);
}

bool iNesConfiguration::HasTrainerData() const
{
  return m_hasTrainerData;
}

const std::vector<uint8_t>& iNesConfiguration::GetTrainerData() const
{
  return m_trainerData;
}

const std::vector<uint8_t>& iNesConfiguration::GetPrgRomData() const
{
  return m_prgRomData;
}

std::vector<uint8_t>& iNesConfiguration::GetChrRomData()
{
  return m_chrRomData;
}

uint8_t iNesConfiguration::getPrgBanks() const
{
  return m_prgBanks;
}

uint8_t iNesConfiguration::getChrBanks() const
{
  return m_chrBanks;
}

bool iNesConfiguration::HasPersistentMemory() const
{
  return m_hasPersistentMemory;
}

uint16_t iNesConfiguration::GetMapperId() const
{
  return m_mapperId;
}

MIRROR iNesConfiguration::GetMirror() const
{
  return m_hwMirror;
}
