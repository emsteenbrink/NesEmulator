#pragma once

#include "Component.h"

#include <map>
#include <cassert>
#include <memory>

class ICartridgeBusHelper;

class Bus
{
public:
  void AddComponent(Component& component);
  void RemoveComponent(Component& component);

  void AddCartridge(const std::shared_ptr<ICartridgeBusHelper>& component);
  void RemoveCartridge();

  bool Read(uint16_t addr, uint8_t& data);
  bool Write(uint16_t addr, uint8_t data);

private:
  std::map<Range, Component*>              m_components;
  std::shared_ptr<ICartridgeBusHelper>     m_cartridge;
};
