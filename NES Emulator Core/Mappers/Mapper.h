#pragma once

#include "iNesConfiguration.h"

#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

class Mapper
{
public:
  Mapper(const iNesConfiguration& configuration);

  virtual bool CpuRead(uint16_t addr, uint8_t& data) = 0;
  virtual bool CpuWrite(uint16_t addr, uint8_t data) = 0;

  virtual bool PpuRead(uint16_t addr, uint8_t& data) = 0;
  virtual bool PpuWrite(uint16_t addr, uint8_t data) = 0;

  virtual void Load() {};
  virtual void Save() {};

protected:
  const iNesConfiguration&  m_configuration;
};
