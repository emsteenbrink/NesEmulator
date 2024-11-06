#include "Mapper_000.h"

#include <cassert>

Mapper_000::Mapper_000(iNesConfiguration& configuration, const fs::path& filePath)
  : Mapper(configuration)
  , m_prgRomData(configuration.GetPrgRomData())
  , m_chrRomData(configuration.GetChrRomData())
  , m_nameTable()
{
  if (configuration.HasPersistentMemory())
  {
    fs::path fileName(filePath);
    fileName.replace_extension("sav");
    m_workRam = std::make_unique<PersistentRAM>(8192, uint16_t(0x6000), uint16_t(0x7FFF), fileName);
  }
}

bool Mapper_000::CpuRead(uint16_t address, uint8_t& data)
{
  if (address >= 0x6000 && address <= 0x7FFF)
  {
    if(m_workRam)
      data = m_workRam->Read(address);
    return true;
  }
  else if (address >= 0x8000)
  {
    uint16_t realAddress = (address & 0x7FFF) % m_prgRomData.size();
    data = m_prgRomData[realAddress];
    return true;
  }
  return false;
}

bool Mapper_000::CpuWrite(uint16_t address, uint8_t data)
{
  if (address >= 0x6000 && address <= 0x7FFF)
  {
    if (m_workRam)
      m_workRam->Write(address, data);
    return true;
  }
  // Can't write
  return false;
}

bool Mapper_000::PpuRead(uint16_t address, uint8_t& data)
{
  address &= 0x3FFF;

  if(address >= 0x0000 && address <= 0x1FFF && m_chrRomData.size())
  {
    uint16_t realAddress = address % m_chrRomData.size();
    data = m_chrRomData[realAddress];
    return true;
  }
  else if (address >= 0x2000 && address <= 0x3EFF)
  {
    address &= 0x0FFF;

    if (m_configuration.GetMirror() == MIRROR::VERTICAL)
    {
      // Vertical
      if (address >= 0x0000 && address <= 0x03FF)
        data = m_nameTable[0][address & 0x03FF];
      if (address >= 0x0400 && address <= 0x07FF)
        data = m_nameTable[1][address & 0x03FF];
      if (address >= 0x0800 && address <= 0x0BFF)
        data = m_nameTable[0][address & 0x03FF];
      if (address >= 0x0C00 && address <= 0x0FFF)
        data = m_nameTable[1][address & 0x03FF];
    }
    else if (m_configuration.GetMirror() == MIRROR::HORIZONTAL)
    {
      // Horizontal
      if (address >= 0x0000 && address <= 0x03FF)
        data = m_nameTable[0][address & 0x03FF];
      if (address >= 0x0400 && address <= 0x07FF)
        data = m_nameTable[0][address & 0x03FF];
      if (address >= 0x0800 && address <= 0x0BFF)
        data = m_nameTable[1][address & 0x03FF];
      if (address >= 0x0C00 && address <= 0x0FFF)
        data = m_nameTable[1][address & 0x03FF];
    }
    return true;
  }
  return false;
}

bool Mapper_000::PpuWrite(uint16_t address, uint8_t data)
{
  address &= 0x3FFF;

  if (address >= 0x0000 && address <= 0x1FFF)
  {
    uint16_t realAddress = address % m_chrRomData.size();
    m_chrRomData[realAddress] = data;
    return true;
  }
  else if (address >= 0x2000 && address <= 0x3EFF)
  {
    address &= 0x0FFF;
    if (m_configuration.GetMirror() == MIRROR::VERTICAL)
    {
      // Vertical
      if (address >= 0x0000 && address <= 0x03FF)
        m_nameTable[0][address & 0x03FF] = data;
      if (address >= 0x0400 && address <= 0x07FF)
        m_nameTable[1][address & 0x03FF] = data;
      if (address >= 0x0800 && address <= 0x0BFF)
        m_nameTable[0][address & 0x03FF] = data;
      if (address >= 0x0C00 && address <= 0x0FFF)
        m_nameTable[1][address & 0x03FF] = data;
    }
    else if (m_configuration.GetMirror() == MIRROR::HORIZONTAL)
    {
      // Horizontal
      if (address >= 0x0000 && address <= 0x03FF)
        m_nameTable[0][address & 0x03FF] = data;
      if (address >= 0x0400 && address <= 0x07FF)
        m_nameTable[0][address & 0x03FF] = data;
      if (address >= 0x0800 && address <= 0x0BFF)
        m_nameTable[1][address & 0x03FF] = data;
      if (address >= 0x0C00 && address <= 0x0FFF)
        m_nameTable[1][address & 0x03FF] = data;
    }
    return true;
  }
  return false;
}

void Mapper_000::Save()
{
  if(m_workRam)
    m_workRam->Save();
}

void Mapper_000::Load()
{
  if (m_workRam)
    m_workRam->Load();
}
