#pragma once

#include <cstdint>

class LinearCounter
{
public:
  LinearCounter();

  void SetControlFlag(bool controlFlag);
  void SetReloadValue(uint8_t value);

  void Clock();

  bool IsZero();

private:
  bool        m_controlFlag;
  bool        m_reload;
  uint8_t     m_reloadValue;

  uint8_t     m_counter;
};