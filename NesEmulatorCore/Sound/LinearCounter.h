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
  bool        m_controlFlag = false;
  bool        m_reload = false;
  uint8_t     m_reloadValue = 0;

  uint8_t     m_counter = 0;
};