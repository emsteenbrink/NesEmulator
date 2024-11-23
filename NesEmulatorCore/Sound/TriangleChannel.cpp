#include "TriangleChannel.h"

const std::vector<uint8_t> TriangleChannel::m_sequenceValues =
{ 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 };



TriangleChannel::TriangleChannel()
  : m_sequencer(1)
  , m_lengthCounter(false)
{
  m_sequencer.SetSequence(&m_sequenceValues);
}

void TriangleChannel::Clock()
{
  if(!m_lengthCounter.Silenced() && !m_linearCounter.IsZero())
    m_sequencer.Clock();
}

void TriangleChannel::DoQuarterFrame()
{
  m_linearCounter.Clock();
}

void TriangleChannel::DoHalfFrame()
{
  m_lengthCounter.Clock();
}

void TriangleChannel::SetEnabled(bool enabled)
{
  m_enabled = enabled;
  m_lengthCounter.Enabled(enabled);
  m_sequencer.SetEnabled(enabled);
}


void TriangleChannel::SetRegister0(uint8_t data)
{
  m_linearCounter.SetReloadValue(data & 0x7F);
  m_linearCounter.SetControlFlag(data & 0x80);
  m_lengthCounter.Halt(data & 0x80);
}

void TriangleChannel::SetRegister2(uint8_t data)
{
  m_sequencer.SetTimerLow(data);
}

void TriangleChannel::SetRegister3(uint8_t data)
{
  m_sequencer.SetTimerHigh(data & 0x07);
  m_lengthCounter.Load(data >> 3);
}

uint8_t TriangleChannel::Output()
{
  return m_enabled ? m_sequencer.GetOutput() : 0;
}

bool TriangleChannel::Status()
{
  return !m_lengthCounter.Silenced();
}
