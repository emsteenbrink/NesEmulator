#pragma once

#include "Component.h"

#include <array>
#include <filesystem>

namespace fs = std::filesystem;

class RAM : public Component
{
public:
  RAM(unsigned int size, uint16_t beginAddress, uint16_t endAddress);

  // Inherited via Component
  virtual uint8_t Read(uint16_t address) override;
  virtual uint8_t Write(uint16_t address, uint8_t data) override;

public:
  std::vector<uint8_t> m_ram;
  uint16_t m_beginAddress;
  uint16_t m_endAddress;
  uint32_t m_size;
};
