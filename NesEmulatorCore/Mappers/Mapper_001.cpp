#include "Mapper_001.h"
#include <iostream>
#include <cassert>

using namespace std;

Mapper_001::Mapper_001(iNesConfiguration& configuration, const fs::path& filePath)
  : Mapper(configuration)
  , m_prgRomData(configuration.GetPrgRomData())
  , m_chrRomData(configuration.GetChrRomData())
  , m_nameTable()
  , m_loadRegister(0)
  , m_loadRegisterCount(0)
  , m_controlRegister()
  , m_chrBank0(0)
  , m_chrBank1(0)
  , m_prgBank(0)
{
  m_controlRegister.raw = 0x1C;
  fs::path fileName(filePath);
  fileName.replace_extension("sav");
  m_workRam = std::make_unique<PersistentRAM>(8192, uint16_t(0x6000), uint16_t(0x7FFF), fileName);
}

bool Mapper_001::CpuRead(uint16_t address, uint8_t& data)
{
  if (address >= 0x6000 && address <= 0x7FFF)
  {
    if (m_workRam)
      data = m_workRam->Read(address);
    return true;
  }
  else if (address >= 0x8000)
  {
    uint32_t realAddress = 0;

    if (m_controlRegister.data.prgBankMode == PrgBankMode::FIX_FIRST)
    {
      if (address >= 0x8000 && address <= 0xBFFF)
        realAddress =  address - 0x8000;
      if (address >= 0xC000 && address <= 0xFFFF)
        realAddress = (m_prgBank & 0x0F) * 0x4000 + address - 0xC000;
    }
    else if (m_controlRegister.data.prgBankMode == PrgBankMode::FIX_LAST)
    {
      if (address >= 0x8000 && address <= 0xBFFF)
        realAddress = (uint32_t(m_prgBank) & 0x0F) * 0x4000 + (address - 0x8000);
      if (address >= 0xC000 && address <= 0xFFFF)
        realAddress = (uint32_t(m_configuration.getPrgBanks()) - 1) * 0x4000 + (address - 0xC000);
    }
    else  // SWITCH32
    {
      realAddress = (m_prgBank >> 1) * 0x8000 + address - 0x8000;
    }
    data = m_prgRomData[realAddress];
    return true;
  }
  return false;
}

bool Mapper_001::CpuWrite(uint16_t address, uint8_t data)
{
  if (address >= 0x6000 && address <= 0x7FFF)
  {
    if (m_workRam)
      m_workRam->Write(address, data);
    return true;
  }

  if (address >= 0x8000)
  {
    if (data & 0x80)
    {
      // MSB is set, so reset serial loading
      m_loadRegister = 0x00;
      m_loadRegisterCount = 0;
      m_controlRegister.raw = m_controlRegister.raw & 0xF3 + 0x0C;
      cout << "Reset ControlRegister" << endl;
    }
    else
    {
      m_loadRegister >>= 1;
      m_loadRegister |= (data & 0x01) << 4;
      ++m_loadRegisterCount;

      if (m_loadRegisterCount == 5)
      {
        cout << "Set register: " << hex << address << " to " << int(m_loadRegister) << endl;
        if (address >= 0x8000 && address <= 0x9FFF)
        {
          m_controlRegister.raw = m_loadRegister;
          cout << "Mirroring: ";
          switch (m_controlRegister.data.mirroring)
          {
          case ONE_SCREEN_LOWER_BANK: cout << "One screen Lower"; break;
          case ONE_SCREEN_UPPER_BANK: cout << "One screen Upper"; break;
          case VERTICAL: cout << "Vertical"; break;
          case HORIZONTAL: cout << "Horizontal"; break;
          }
          cout << endl;
          cout << "Chr :";
          switch (m_controlRegister.data.chrBankMode)
          {
          case ChrBankMode::SWITCH_8KB: cout << "8Kb"; break;
          case ChrBankMode::SWITCH_4KB: cout << "4Kb"; break;
          }
          cout << endl;
          cout << "Prg :";
          switch (m_controlRegister.data.prgBankMode)
          {
          case SWITCH32: cout << "32"; break;
          case SWITCH32_: cout << "32_"; break;
          case FIX_FIRST: cout << "Fix_First"; break;
          case FIX_LAST: cout << "Fix_Last"; break;
          }
          cout << endl;
        }
        else if (address >= 0xA000 && address <= 0xBFFF)
          m_chrBank0 = m_loadRegister;
        else if (address >= 0xC000 && address <= 0xDFFF)
          m_chrBank1 = m_loadRegister;
        else if (address >= 0xE000 && address <= 0xFFFF)
          m_prgBank = m_loadRegister;

        m_loadRegister = 0x00;
        m_loadRegisterCount = 0;
      }
    }
    return true;
  }
  // Can't write
  return false;
}

bool Mapper_001::PpuRead(uint16_t address, uint8_t& data)
{
  address &= 0x3FFF;

  if(address <= 0x1FFF && m_chrRomData.size())
  {
    if (m_configuration.getChrBanks() == 0)
      data = m_chrRomData[address];
    else if (m_controlRegister.data.chrBankMode == ChrBankMode::SWITCH_4KB)
    {
      if (address >= 0x0000 && address <= 0x0FFF)
        data = m_chrRomData[uint32_t(m_chrBank0) * 0x1000 + address];
      else if (address >= 0x1000 && address <= 0x1FFF)
        data = m_chrRomData[uint32_t(m_chrBank1) * 0x1000 + address - 0x1000];
    }
    else if (m_controlRegister.data.chrBankMode == ChrBankMode::SWITCH_8KB)
      data = m_chrRomData[(uint32_t(m_chrBank0) >> 1) * 0x2000 + address];

    return true;
  }
  else if (address >= 0x2000 && address <= 0x3EFF)
  {
    address &= 0x0FFF;

    if (m_controlRegister.data.mirroring == Mirroring::VERTICAL)
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
    else if (m_controlRegister.data.mirroring == Mirroring::HORIZONTAL)
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
    else if (m_controlRegister.data.mirroring == Mirroring::ONE_SCREEN_LOWER_BANK)
    {
      data = m_nameTable[0][address & 0x03FF];
    }
    else if (m_controlRegister.data.mirroring == Mirroring::ONE_SCREEN_UPPER_BANK)
    {
      data = m_nameTable[1][address & 0x03FF];
    }
    return true;
  }
  return false;
}

bool Mapper_001::PpuWrite(uint16_t address, uint8_t data)
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
    if (m_controlRegister.data.mirroring == Mirroring::VERTICAL)
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
    else if (m_controlRegister.data.mirroring == Mirroring::HORIZONTAL)
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
    else if (m_controlRegister.data.mirroring == Mirroring::ONE_SCREEN_LOWER_BANK)
    {
      m_nameTable[0][address & 0x03FF] = data;
    }
    else if (m_controlRegister.data.mirroring == Mirroring::ONE_SCREEN_UPPER_BANK)
    {
      m_nameTable[1][address & 0x03FF] = data;
    }
    return true;
  }
  return false;
}

void Mapper_001::Save()
{
  if (m_configuration.HasPersistentMemory())
    m_workRam->Save();
}

void Mapper_001::Load()
{
  if (m_configuration.HasPersistentMemory())
    m_workRam->Load();
}

