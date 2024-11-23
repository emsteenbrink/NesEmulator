#include "Sweep.h"
#include "Sequencer.h"

Sweep::Sweep(Sequencer& sequencer, bool add1sComplement)
  : m_sequencer(sequencer)
  , m_add1sComplement(add1sComplement)
{
}

void Sweep::SetEnabled(bool enabled)
{
  m_enabled = enabled;
  m_reload = true;
}

void Sweep::SetDividerPeriod(uint8_t dividerPeriod)
{
  m_dividerPeriod = dividerPeriod;
  m_reload = true;
}

void Sweep::SetNegateFlag(bool negateFlag)
{
  m_negateFlag = negateFlag;
  m_reload = true;
}

void Sweep::SetShiftCount(uint8_t shiftCount)
{
  m_shiftCount = shiftCount;
  m_reload = true;
}

void Sweep::Clock()
{
  uint16_t currentTimerLoad = m_sequencer.TimerLoad();
  bool mute = currentTimerLoad < 8 || currentTimerLoad > 0x7FFF;

  if (m_divider == 0 && m_enabled && !mute)
  {
    int16_t change = (int16_t)(currentTimerLoad >> m_shiftCount);
    if (m_negateFlag)
    {
      change += m_add1sComplement ? 1 : 0;
      m_sequencer.TimerLoad(currentTimerLoad - change);
    }
    else
    {
      m_sequencer.TimerLoad(currentTimerLoad + change);
    }
  }

  if (m_divider == 0 || m_reload)
  {
    m_reload = false;
    m_divider = m_dividerPeriod;
  }
  else
    --m_divider;
}
