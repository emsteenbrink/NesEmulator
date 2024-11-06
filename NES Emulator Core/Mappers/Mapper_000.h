#pragma once

#include "Mapper.h"
#include "PersistentRAM.h"

#include "iNesConfiguration.h"

class Mapper_000 : public Mapper
{
public:
  Mapper_000(iNesConfiguration& configuration, const fs::path& filePath);

  // Inherited via IMapper
  virtual bool CpuRead(uint16_t address, uint8_t& data) override;
  virtual bool CpuWrite(uint16_t address, uint8_t data) override;
  virtual bool PpuRead(uint16_t address, uint8_t& data) override;
  virtual bool PpuWrite(uint16_t address, uint8_t data) override;

  virtual void Save() override;
  virtual void Load() override;

private:
  const std::vector<uint8_t>&     m_prgRomData;
  std::vector<uint8_t>&           m_chrRomData;
  std::unique_ptr<PersistentRAM>  m_workRam;

  uint8_t                         m_nameTable[2][1024];
};
