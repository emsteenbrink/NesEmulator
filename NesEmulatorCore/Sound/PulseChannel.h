#pragma once

#include "IChannel.h"

#include "Sequencer.h"
#include "Envelope.h"
#include "LengthCounter.h"
#include "Sweep.h"

#include <cstdint>
#include <vector>
#include <array>

class PulseChannel : public IChannel
{
public:
  PulseChannel(bool isPulse1);

  // Inherited via IChannel
  virtual void Clock() override;
  virtual void DoQuarterFrame() override;
  virtual void DoHalfFrame() override;

  void SetEnabled(bool enabled);

  uint8_t Output();
  bool Status();

  void SetRegister0(uint8_t data);
  void SetRegister1(uint8_t data);
  void SetRegister2(uint8_t data);
  void SetRegister3(uint8_t data);

private:
  bool            m_enabled = false;
  Sequencer       m_sequencer;
  Envelope        m_envelope;
  LengthCounter   m_lengthCounter;
  Sweep           m_sweep;

  static const std::array<std::vector<uint8_t>, 4> m_dutyCycles;
};