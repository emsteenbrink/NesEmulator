#include "APU.h"

#include <algorithm>


APU::APU(Bus& cpuBus, ISoundSampleProcessor& soundSampleProcessor)
  : m_pulse1(true)
  , m_pulse2(false)
  , m_dmc(cpuBus)
  , m_soundSampleProcessor(soundSampleProcessor)
{
  AddAddressRange(0x4000, 0x4013);
  AddAddress(0x4015);
  AddAddress(0x4017);

  m_pulseTable[0] = 0;
  for (int i = 1; i < 31; ++i)
  {
    m_pulseTable[i] = (uint16_t)(95.52f / (8128.0f / i + 100.0f) * UINT16_MAX/2);
  }

  m_tndTable[0] = 0;
  for (int i = 1; i < 203; ++i)
  {
    m_tndTable[i] = (uint16_t)(163.67f / (24329.0f / i + 100.0f) * UINT16_MAX/2);
  }

  m_channels.push_back(&m_pulse1);
  m_channels.push_back(&m_pulse2);
  m_channels.push_back(&m_triangle);
  m_channels.push_back(&m_noise);
  m_channels.push_back(&m_dmc);
}

APU::~APU()
{
}

void APU::Clock()
{
  uint16_t currentCycle =  m_time % (m_5stepMode ? Mode5TotalTicks : Mode4TotalTicks);
  if (m_sequenceResetTimer > 0)
  {
    if (--m_sequenceResetTimer == 0)
    {
      m_time = 0;
      if (m_5stepMode)
      {
        std::for_each(begin(m_channels), end(m_channels), [](IChannel* i) {i->DoHalfFrame(); });
        std::for_each(begin(m_channels), end(m_channels), [](IChannel* i) {i->DoQuarterFrame(); });
      }
    }
  }

  if ( currentCycle == SequenceStep1
    || currentCycle == SequenceStep3)
  {
    std::for_each(begin(m_channels), end(m_channels), [](IChannel* i) {i->DoQuarterFrame(); });
  }
  else if ( currentCycle == SequenceStep2
         || (!m_5stepMode && currentCycle == SequenceStep4)
         || (m_5stepMode && currentCycle == SequenceStep5))
  {
    std::for_each(begin(m_channels), end(m_channels), [](IChannel* i) {i->DoHalfFrame(); });
    std::for_each(begin(m_channels), end(m_channels), [](IChannel* i) {i->DoQuarterFrame(); });
  }

  if (!m_5stepMode && currentCycle == SequenceStep4 && !m_interuptInhibit)
  {
    m_frameInterrupt = true;
  }

  if (m_time % 2 == 0)
  {
    m_pulse1.Clock();
    m_pulse2.Clock();
    m_noise.Clock();
    //std::for_each(begin(m_channels), end(m_channels), [](IChannel* i) {i->Clock(); });
    int32_t pulseOut = m_pulseTable[m_pulse1.Output() + m_pulse2.Output()];
    int32_t tndOut = m_tndTable[3 * m_triangle.Output() + 2 * m_noise.Output() + m_dmc.Output()];
    m_soundSampleProcessor.AddSample(int16_t((pulseOut + tndOut)));
  }

  m_triangle.Clock();
  m_dmc.Clock();
  ++m_time;
}

uint8_t APU::Read(uint16_t address)
{
  switch (address)
  {
    // only register 0x4015 can be read.
  case 0x4015:
    bool frameInterrupt = m_frameInterrupt;
    m_frameInterrupt = false; // reset when read.
    return  m_dmc.Interrupt() << 7
      | frameInterrupt << 6
      | m_dmc.Status() << 4
      | m_noise.Status() << 3
      | m_triangle.Status() << 2
      | m_pulse2.Status() << 1
      | m_pulse1.Status() << 0;
    break;
  }
  return 0x00;
}

uint8_t APU::Write(uint16_t address, uint8_t data)
{
  switch (address)
  {
  case 0x4000: m_pulse1.SetRegister0(data); break;
  case 0x4001: m_pulse1.SetRegister1(data); break;
  case 0x4002: m_pulse1.SetRegister2(data); break;
  case 0x4003: m_pulse1.SetRegister3(data); break;
  case 0x4004: m_pulse2.SetRegister0(data); break;
  case 0x4005: m_pulse2.SetRegister1(data); break;
  case 0x4006: m_pulse2.SetRegister2(data); break;
  case 0x4007: m_pulse2.SetRegister3(data); break;
  case 0x4008: m_triangle.SetRegister0(data); break;
  case 0x4009: break; // unused
  case 0x400a: m_triangle.SetRegister2(data); break;
  case 0x400b: m_triangle.SetRegister3(data); break;
  case 0x400c: m_noise.SetRegister0(data); break;
  case 0x400d: break; // unused
  case 0x400e: m_noise.SetRegister2(data); break;
  case 0x400f: m_noise.SetRegister3(data); break;
  case 0x4010: m_dmc.SetRegister0(data); break;
  case 0x4011: m_dmc.SetRegister1(data); break;
  case 0x4012: m_dmc.SetRegister2(data); break;
  case 0x4013: m_dmc.SetRegister3(data); break;
  case 0x4015:
    m_pulse1.SetEnabled(data & 0x01);
    m_pulse2.SetEnabled(data & 0x02);
    m_triangle.SetEnabled(data & 0x04);
    m_noise.SetEnabled(data & 0x08);
    m_dmc.SetEnabled(data & 0x10);
    break;
  case 0x4017:
    m_5stepMode = data & 0x80;
    m_sequenceResetTimer = 4;
    m_interuptInhibit = data & 0x40;
    m_time = 0;
    if (m_5stepMode)
    {
      //m_pulse1.Clock();
      //m_pulse2.Clock();
      //m_triangle.Clock();
      //m_noise.Clock();
      //m_dmc.Clock();
      m_pulse1.DoHalfFrame();
      m_pulse2.DoHalfFrame();
      m_triangle.DoHalfFrame();
      m_noise.DoHalfFrame();
      m_dmc.DoHalfFrame();
    }
    break;
  }
  return 0xFF;
}

