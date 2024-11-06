#include "NoiseChannel.h"

const std::array<uint16_t, 16> NoiseChannel::m_timerPeriods =
    { 4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068 };

NoiseChannel::NoiseChannel()
{
}

void NoiseChannel::SetEnabled(bool enabled)
{
  m_enabled = enabled;
  m_lengthCounter.Enabled(enabled);
  m_envelope.Restart();
}

void NoiseChannel::SetRegister0(uint8_t data)
{
  m_envelope.UseConstantVolume(data & 0x10);
  m_envelope.Loop(data & 0x20);
  m_envelope.EnvelopeParameter(data & 0x0f);
  m_lengthCounter.Halt(data & 0x20);

}

void NoiseChannel::SetRegister2(uint8_t data)
{
  m_mode = data & 0x80;
  m_timerReload = m_timerPeriods[data & 0x0f];
  m_timer = m_timerReload;
}

void NoiseChannel::SetRegister3(uint8_t data)
{
  m_lengthCounter.Load(data >> 3);
  m_envelope.Restart();
}


uint8_t NoiseChannel::Output()
{
  return m_enabled && !m_lengthCounter.Silenced() && (m_shiftRegister & 0x01) == 0 ? m_envelope.GetOutput() : 0;
}

bool NoiseChannel::Status()
{
    return !m_lengthCounter.Silenced();
}

void NoiseChannel::Clock()
{
  --m_timer;
  if (m_timer == 0xFFFF)
  {
    m_timer = m_timerReload;
    uint16_t feedback = (m_shiftRegister << 14) ^ (m_shiftRegister << (m_mode ? 9 : 13)); // either bit 1 or bit 6 is shifted to the correct position
    feedback &= 0x4000;
    m_shiftRegister = feedback | (m_shiftRegister >> 1);
  }
}

void NoiseChannel::DoQuarterFrame()
{
  m_envelope.Clock();
}

void NoiseChannel::DoHalfFrame()
{
  m_lengthCounter.Clock();
}
