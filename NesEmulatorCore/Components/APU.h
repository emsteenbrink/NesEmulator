#pragma once

#include "Component.h"
#include "PulseChannel.h"
#include "TriangleChannel.h"
#include "NoiseChannel.h"
#include "DMCChannel.h"
#include "ISoundSampleProcessor.h"
#include "Bus.h"

class APU : public Component
{
public:
  APU(Bus& cpuBus, ISoundSampleProcessor& soundSampleProcessor);
  virtual ~APU();

  void Clock();

private:
  // Inherited via CpuComponent
  virtual uint8_t Read(uint16_t addr) override;
  virtual uint8_t Write(uint16_t addr, uint8_t data) override;

  PulseChannel                m_pulse1;
  PulseChannel                m_pulse2;
  TriangleChannel             m_triangle;
  NoiseChannel                m_noise;
  DMCChannel                  m_dmc;

  std::array<uint16_t, 31>    m_pulseTable;
  std::array<uint16_t, 203>   m_tndTable;

  ISoundSampleProcessor&      m_soundSampleProcessor;

  bool                        m_5stepMode = false;
  bool                        m_interuptInhibit = false;

  long long                   m_time = 0;

  static const uint16_t SequenceStep1 = 7457;    // 2 * 3728.5
  static const uint16_t SequenceStep2 = 14913;   // 2 * 7456.5
  static const uint16_t SequenceStep3 = 22371;   // 2 * 11185.5
  static const uint16_t SequenceStep4 = 29829;   // 2 * 14914.5
  static const uint16_t SequenceStep5 = 37281;   // 2 * 18640.5

  static const uint16_t Mode4TotalTicks = 29830; // 2 * 14915
  static const uint16_t Mode5TotalTicks = 37282; // 2 * 18641

  uint8_t m_sequenceResetTimer = 0;

  std::vector<IChannel*> m_channels;

  bool m_frameInterrupt = false;
};
