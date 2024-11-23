#include "DMCChannel.h"

const std::array<uint16_t, 16> DMCChannel::m_ntscRates = 
  { 428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54 };
const std::array<uint16_t, 16> DMCChannel::m_palRates = 
  { 398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118,  98,  78,  66,  50 };


DMCChannel::DMCChannel(Bus& cpuBus)
  : m_cpuBus(cpuBus)
{
}

void DMCChannel::SetEnabled(bool enabled)
{
  m_enabled = enabled;
}

void DMCChannel::SetRegister0(uint8_t data)
{
  m_irqEnabled = data & 0x80;
  m_loop = data & 0x40;
  m_rate = m_pal ? m_palRates[data] : m_ntscRates[data];
}

void DMCChannel::SetRegister1(uint8_t data)
{
  m_output = data & 0x7f;
}

void DMCChannel::SetRegister2(uint8_t data)
{
  m_sampleAddress = 0xc000 | (uint16_t)data << 6;
}

void DMCChannel::SetRegister3(uint8_t data)
{
  m_sampleLength = (uint16_t)data << 4 | 0x01;
}

uint8_t DMCChannel::Output()
{
  return m_output;
}

bool DMCChannel::Status()
{
  return m_samplesLeft > 0;
}

bool DMCChannel::Interrupt()
{
  return m_interrupt;
}

void DMCChannel::Clock()
{
  if (m_enabled)
  {
    if (m_buffered && m_samplesLeft)
    {
      // time to load a new byte.
      m_cpuBus.Read(m_currentAddress, m_sampleBuffer);
      if (!(--m_samplesLeft))
      {
        if (m_loop)
        {
          m_currentAddress = m_sampleAddress;
          m_samplesLeft = m_sampleLength;
        }
        else
          m_interrupt = true;
      }
      else
      {
        if (m_currentAddress == 0xffff)
          m_currentAddress = 0x8000;
        else
          ++m_currentAddress;
      }
    }
    if (m_timer)
      --m_timer;
    else
    {
      m_timer = m_rate;
      if (m_sampleBitsLeft == 0)
      {
        m_sampleBitsLeft = 8;
        if (!m_buffered)
          m_silenced = true;
        else
        {
          m_silenced = false;
          m_buffered = false;
          m_shiftReg = m_sampleBuffer;
        }
      }
      if (!m_silenced)
      {
        if ((m_output > 1) && !(m_shiftReg & 0x01))
          m_output -= 2;
        else
          if ((m_output < 126) && (m_shiftReg & 0x01))
            m_output += 2;
      }
      m_shiftReg >>= 1;
      --m_sampleBitsLeft;
    }
  }
}

void DMCChannel::DoQuarterFrame()
{
  // Do nothing.
}

void DMCChannel::DoHalfFrame()
{
  // Do nothing.
}
