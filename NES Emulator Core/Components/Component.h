#pragma once

#include <cstdint>
#include <vector>
#include "Range.h"

class Component
{
public:
  Component();
  Component(Range range);
  virtual ~Component() {};

  virtual uint8_t Read(uint16_t address) = 0;
  virtual void Write(uint16_t address, uint8_t data) = 0;

  const std::vector<Range>& GetAddressRanges() const;

protected:
  void AddAddressRange(Range range);
  void AddAddressRange(uint16_t start, uint16_t end);
  void AddAddress(uint16_t address);

private:
  std::vector<Range> m_addressRanges;
};
