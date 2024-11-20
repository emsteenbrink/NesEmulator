#pragma once

#include "IChannel.h"
#include "Bus.h"

#include <cstdint>
#include <array>

class DMCChannel : public IChannel
{
public:
  DMCChannel(Bus& cpuBus);

  void SetEnabled(bool enabled);

  void SetRegister0(uint8_t data);
  void SetRegister1(uint8_t data);
  void SetRegister2(uint8_t data);
  void SetRegister3(uint8_t data);

  uint8_t Output();
  bool Status();

  bool Interrupt();

  // Inherited via IChannel
  virtual void Clock() override;
  virtual void DoQuarterFrame() override;
  virtual void DoHalfFrame() override;


private:
  Bus& m_cpuBus;

  bool m_pal = false;
  bool m_enabled = false;
  bool m_irqEnabled = false;
  bool m_loop = false;
  uint16_t m_rate = 0;
  
  uint16_t m_sampleAddress = 0;
  uint16_t m_sampleLength = 0;

  uint16_t m_currentAddress = 0;
  uint16_t m_samplesLeft = 0;

  uint8_t m_sampleBitsLeft = 0;
  uint8_t m_sampleBuffer = 0;

  uint8_t m_shiftReg = 0;
  uint8_t m_output = 0;

  uint16_t m_timer = 0;
  bool m_buffered = false;
  bool m_silenced = true;

  static const std::array<uint16_t, 16> m_ntscRates;
  static const std::array<uint16_t, 16> m_palRates;

  bool m_interrupt = false;
};