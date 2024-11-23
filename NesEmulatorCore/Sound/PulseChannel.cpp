#include "PulseChannel.h"

const std::array<std::vector<uint8_t>, 4> PulseChannel::m_dutyCycles = 
  {{ {0, 1, 0, 0, 0, 0, 0, 0}     // 12.5 %
   , {0, 1, 1, 0, 0, 0, 0, 0}     // 25 %
   , {0, 1, 1, 1, 1, 0, 0, 0}     // 50 %
   , {1, 0, 0, 1, 1, 1, 1, 1} }}; // 25 % negated

PulseChannel::PulseChannel(bool isPulse1)
  : m_sequencer(0)
  , m_sweep(m_sequencer, isPulse1)
{
}

void PulseChannel::SetEnabled(bool enabled)
{
  m_sequencer.SetEnabled(enabled);
  m_lengthCounter.Enabled(enabled);
  m_enabled = true;
}

uint8_t PulseChannel::Output()
{
  return m_enabled && !m_lengthCounter.Silenced() && m_sequencer.GetOutput() ? m_envelope.GetOutput() : 0;
}

bool PulseChannel::Status()
{
  return !m_lengthCounter.Silenced();
}

void PulseChannel::SetRegister0(uint8_t data)
{
  m_sequencer.SetSequence(&m_dutyCycles[data >> 6]);
  m_envelope.EnvelopeParameter(data & 0x0F);
  m_envelope.UseConstantVolume(data & 0x10);
  m_envelope.Loop(data & 0x20);
  m_lengthCounter.Halt(data & 0x20);
}

void PulseChannel::SetRegister1(uint8_t data)
{
  m_sweep.SetEnabled(data & 0x80);
  m_sweep.SetDividerPeriod((data >> 4) & 0x07);
  m_sweep.SetNegateFlag(data & 0x08);
  m_sweep.SetShiftCount(data & 0x07);
}

void PulseChannel::SetRegister2(uint8_t data)
{
  m_sequencer.SetTimerLow(data);
}

void PulseChannel::SetRegister3(uint8_t data)
{
  m_sequencer.SetTimerHigh(data & 0x07);
  m_lengthCounter.Load(data >> 3);
  m_envelope.Restart();
}

void PulseChannel::Clock()
{
  m_sequencer.Clock();
}

void PulseChannel::DoQuarterFrame()
{
  //  Envelopes & triangle's linear counter (Quarter frame)
  m_envelope.Clock();
}

void PulseChannel::DoHalfFrame()
{
  // Length counters & sweep units (Half frame)
  m_lengthCounter.Clock();
  m_sweep.Clock();
}
