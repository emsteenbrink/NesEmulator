#pragma once

#include "Mapper.h"
#include "PersistentRAM.h"

#include "iNesConfiguration.h"


class Mapper_001 : public Mapper
{
public:
  Mapper_001(iNesConfiguration& configuration, const fs::path& filePath);

  // Inherited via IMapper
  virtual bool CpuRead(uint16_t address, uint8_t& data) override;
  virtual bool CpuWrite(uint16_t address, uint8_t data) override;
  virtual bool PpuRead(uint16_t address, uint8_t& data) override;
  virtual bool PpuWrite(uint16_t address, uint8_t data) override;

  virtual void Save() override;
  virtual void Load() override;

private:
  enum Mirroring : uint8_t
  {
    ONE_SCREEN_LOWER_BANK = 0,
    ONE_SCREEN_UPPER_BANK = 1,
    VERTICAL = 2,
    HORIZONTAL = 3
  };

  enum PrgBankMode : uint8_t
  {
    SWITCH32 = 0,
    SWITCH32_ = 1,
    FIX_FIRST = 2,
    FIX_LAST = 3
  };

  enum ChrBankMode : uint8_t
  {
    SWITCH_8KB = 0, 
    SWITCH_4KB = 1
  };
  
  union {
    uint8_t raw;

    struct
    {
      Mirroring     mirroring : 2;
      PrgBankMode   prgBankMode : 2;
      ChrBankMode   chrBankMode : 1;
      uint8_t       unused : 3;
    } data;
  } m_controlRegister;


  const std::vector<uint8_t>&     m_prgRomData;
  std::vector<uint8_t>&           m_chrRomData;
  std::unique_ptr<PersistentRAM>  m_workRam;

  uint8_t                         m_nameTable[2][1024];

  uint8_t                         m_loadRegister = 0x00;
  uint8_t                         m_loadRegisterCount = 0;

  uint8_t                         m_chrBank0;
  uint8_t                         m_chrBank1;
  uint8_t                         m_prgBank;
};
