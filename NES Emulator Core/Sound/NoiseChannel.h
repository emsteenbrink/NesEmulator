#pragma once

#include "IChannel.h"

#include "Envelope.h"
#include "LengthCounter.h"


#include <cstdint>
#include <array>

class NoiseChannel : public IChannel
{
public:
  NoiseChannel();

  void SetEnabled(bool enabled);

  void SetRegister0(uint8_t data);
  void SetRegister2(uint8_t data);
  void SetRegister3(uint8_t data);

  uint8_t Output();
  bool Status();

  // Inherited via IChannel
  virtual void Clock() override;
  virtual void DoQuarterFrame() override;
  virtual void DoHalfFrame() override;

private:
  bool              m_enabled = false;
  uint16_t          m_timerReload = 0;
  bool              m_mode = false;

  Envelope          m_envelope;
  LengthCounter     m_lengthCounter;
  uint16_t          m_timer = 0;
  uint16_t          m_shiftRegister = 1;

  static const std::array<uint16_t, 16> m_timerPeriods;

};