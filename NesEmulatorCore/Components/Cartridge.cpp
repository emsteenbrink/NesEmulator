#include "Cartridge.h"

#include <filesystem>

using namespace std;

Cartridge::Cartridge(Bus& cpuBus, Bus& ppuBus, const std::filesystem::path& filePath)
  : m_cpuBus(cpuBus)
  , m_ppuBus(ppuBus)
  , m_configuration(filePath)
{
  if (!m_configuration.isValid())
  {
    m_isValid = false;
    m_error = std::format("Failed loading cartridge: {}", m_configuration.getError());
    return;
  }
  m_mapper = MapperFactory::CreateMapper(m_configuration.GetMapperId(), m_configuration, filePath);
  if (!m_mapper)
  {
    m_isValid = false;
    m_error = std::format("Mapper ID: {} is not supported", m_configuration.GetMapperId());
    return;
  }
  m_mapper->Load();

  m_cpuBusHelper = make_shared<CpuBusHelper>(m_mapper);
  m_ppuBusHelper = make_shared<PpuBusHelper>(m_mapper);

  m_cpuBus.AddCartridge(m_cpuBusHelper);
  m_ppuBus.AddCartridge(m_ppuBusHelper);
}

Cartridge::~Cartridge()
{
  m_cpuBus.RemoveCartridge();
  m_ppuBus.RemoveCartridge();

  if (m_mapper)
    m_mapper->Save();

  m_cpuBusHelper.reset();
  m_ppuBusHelper.reset();
}

bool Cartridge::isValid() const
{
  return m_isValid;
}

const std::string& Cartridge::getError() const
{
  return m_error;
}
