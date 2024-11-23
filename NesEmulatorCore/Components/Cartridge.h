#pragma once

#include "Bus.h"
#include "MapperFactory.h"
#include "Component.h"

#include <memory>
#include <string>
#include <filesystem>

class ICartridgeBusHelper
{
public:
  virtual bool Read(uint16_t address, uint8_t& data) = 0;
  virtual bool Write(uint16_t address, uint8_t data) = 0;
};

class CpuBusHelper : public ICartridgeBusHelper
{
public:
  CpuBusHelper(std::shared_ptr<Mapper> mapper)
    : m_mapper(mapper)
  {}

private:
  // Inherited via Component
  virtual bool Read(uint16_t address, uint8_t& data) override { return m_mapper ? m_mapper->CpuRead(address, data): false; }
  virtual bool Write(uint16_t address, uint8_t data) override { return m_mapper ? m_mapper->CpuWrite(address, data) : false; }

  std::shared_ptr<Mapper> m_mapper;
};

class PpuBusHelper : public ICartridgeBusHelper
{
public:
  PpuBusHelper(std::shared_ptr<Mapper> mapper)
    : m_mapper(mapper)
  {}

private:
  // Inherited via Component
  virtual bool Read(uint16_t address, uint8_t& data) override { return m_mapper ? m_mapper->PpuRead(address, data) : false; }
  virtual bool Write(uint16_t address, uint8_t data) override { return m_mapper ? m_mapper->PpuWrite(address, data) : false; }

  std::shared_ptr<Mapper> m_mapper;
};

class Cartridge
{
public:
  Cartridge(Bus& cpuBus, Bus& ppuBus, const std::filesystem::path& filePath);
  ~Cartridge();

private:
  iNesConfiguration             m_configuration;
  std::shared_ptr<CpuBusHelper> m_cpuBusHelper;
  std::shared_ptr<PpuBusHelper> m_ppuBusHelper;
  Bus&                          m_cpuBus;
  Bus&                          m_ppuBus;
  std::shared_ptr<Mapper>       m_mapper;
};
