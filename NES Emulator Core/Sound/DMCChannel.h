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
  bool m_enabled;
  bool m_irqEnabled;
  bool m_loop;
  uint16_t m_rate;
  
  uint16_t m_sampleAddress;
  uint16_t m_sampleLength;

  uint16_t m_currentAddress;
  uint16_t m_samplesLeft;

  uint8_t m_sampleBitsLeft;
  uint8_t m_sampleBuffer;

  uint8_t m_shiftReg;
  uint8_t m_output = 0;

  uint16_t m_timer = 0;
  bool m_buffered = false;
  bool m_silenced = true;

  static const std::array<uint16_t, 16> m_ntscRates;
  static const std::array<uint16_t, 16> m_palRates;

  bool m_interrupt;
};