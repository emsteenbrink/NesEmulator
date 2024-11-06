#include "Component.h"

Component::Component()
{
}

Component::Component(Range range)
{
  AddAddressRange(range);
}

const std::vector<Range>& Component::GetAddressRanges() const
{
  return m_addressRanges;
}

void Component::AddAddressRange(Range range)
{
  m_addressRanges.push_back(range);
}

void Component::AddAddressRange(uint16_t start, uint16_t end)
{
  m_addressRanges.emplace_back(start, end);
}

void Component::AddAddress(uint16_t address)
{
  m_addressRanges.emplace_back(address);
}
