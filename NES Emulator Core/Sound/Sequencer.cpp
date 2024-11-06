#include "Sequencer.h"

Sequencer::Sequencer(uint8_t extraTicks)
  : m_extraTicks(extraTicks)
{
}

void Sequencer::SetEnabled(bool enabled)
{
  m_enabled = enabled;
}

void Sequencer::SetSequence(const std::vector<uint8_t>* sequence)
{
  m_sequence = sequence;
}

void Sequencer::SetTimerLow(uint8_t timerLow)
{
  m_timerLoad = ((m_timerLoad & 0xff00) | timerLow);
}

void Sequencer::SetTimerHigh(uint8_t timerHigh)
{
  m_timerLoad = (m_timerLoad & 0x00ff) | ((uint16_t)timerHigh << 8);
  m_currentSequenceIndex = 0;
}

uint8_t Sequencer::GetOutput()
{
  return (m_enabled && m_timerLoad > 8 && m_sequence != nullptr) ? (*m_sequence)[m_currentSequenceIndex] : uint8_t(0);
}

void Sequencer::Clock()
{
  if (m_enabled && m_sequence != nullptr)
  {
    m_timer--;
    if (m_timer == 0xFFFF)
    {
      m_timer = m_timerLoad + m_extraTicks;
      m_currentSequenceIndex = (m_currentSequenceIndex + 1) % m_sequence->size();
    }
  }
}

uint16_t Sequencer::TimerLoad()
{
  return m_timerLoad;
}

void Sequencer::TimerLoad(uint16_t timerLoad)
{
  m_timerLoad = timerLoad;
}
