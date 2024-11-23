#include "Bus.h"
#include "Cartridge.h"

#include <algorithm>

using namespace std;

void Bus::AddComponent(Component& component)
{
  const auto& ranges = component.GetAddressRanges();
  for (const auto& range : ranges)
  {
    m_components.emplace(range, &component);
  }  
}

void Bus::RemoveComponent(Component& component)
{
  for (auto it = end(m_components); it != begin(m_components); --it)
  {
    if (it->second == &component)
    {
      m_components.erase(it);
    }
  }  
}

void Bus::AddCartridge(const shared_ptr<ICartridgeBusHelper>& component)
{
  m_cartridge = component;
}

void Bus::RemoveCartridge()
{
  m_cartridge.reset();
}

bool Bus::Read(uint16_t addr, uint8_t& data)
{
  if (m_cartridge && m_cartridge->Read(addr, data))
  {
    return true;
  }
  else
  {
    auto component = std::find_if(begin(m_components), end(m_components), [addr](auto& item) {return addr >= item.first.low() && addr <= item.first.high(); });
    if (component != end(m_components))
    {
      data = component->second->Read(addr);
      return true;
    }

    //for (auto it = m_components.begin(); it != m_components.end(); ++it)
    //{
    //  auto& range = it->first;
    //  if (addr >= range.low() && addr <= range.high())
    //  {
    //    data = it->second->Read(addr);
    //    return true;
    //  }
    //}
  }
  return false;
}

bool Bus::Write(uint16_t addr, uint8_t data)
{
  if (m_cartridge && m_cartridge->Write(addr, data))
  {
    return true;
  }
  else
  {
    for (auto it = m_components.begin(); it != m_components.end(); ++it)
    {
      auto& range = it->first;
      if (addr >= range.low() && addr <= range.high())
      {
        it->second->Write(addr, data);
        return true;
      }
    }
  }
  return false;
}
