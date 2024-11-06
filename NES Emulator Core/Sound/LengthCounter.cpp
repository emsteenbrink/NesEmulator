#include "LengthCounter.h"

const std::array<uint8_t, 32> LengthCounter::m_LengthTable = 
  { 10, 254, 20, 2 , 40,  4, 80, 6 , 160, 8 , 60, 10, 14, 12, 26, 14
  , 12, 16 , 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

LengthCounter::LengthCounter(bool extraCycle)
  : m_enabled(false)
  , m_halt(false)
  , m_lengthCounter(0)
  , m_extraCycle(extraCycle ? 1: 0)
{
}

void LengthCounter::Enabled(bool enabled)
{
  if (!enabled)
  {
    m_lengthCounter = 0;
  }

  m_enabled = enabled;
}

void LengthCounter::Halt(bool halt)
{
  m_halt = halt;
}

void LengthCounter::Load(uint8_t value)
{
  if (m_enabled)
  {
    m_lengthCounter = m_LengthTable[value] + m_extraCycle;
  }
}

void LengthCounter::Clock()
{
  if (!m_halt && m_lengthCounter != 0)
    --m_lengthCounter;
}

bool LengthCounter::Silenced()
{
  return m_lengthCounter == 0;
}
