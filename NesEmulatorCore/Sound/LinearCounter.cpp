#include "LinearCounter.h"

LinearCounter::LinearCounter()
{
}

void LinearCounter::SetControlFlag(bool controlFlag)
{
  m_controlFlag = controlFlag;
  m_reload = true;
}

void LinearCounter::SetReloadValue(uint8_t value)
{
  m_reloadValue = value;
  m_reload = true;
}


void LinearCounter::Clock()
{
  if (m_reload)
  {
    m_counter = m_reloadValue;
    if (!m_controlFlag)
      m_reload = false;
  }
  else if (m_counter > 0)
  {
    --m_counter;
  }
}

bool LinearCounter::IsZero()
{
  return m_counter == 0;
}
