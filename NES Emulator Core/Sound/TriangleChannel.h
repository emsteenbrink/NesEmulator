#pragma once

#include "IChannel.h"
#include "Sequencer.h"
#include "LengthCounter.h"
#include "LinearCounter.h"

#include <cstdint>

class TriangleChannel : public IChannel
{
public:
  TriangleChannel();

  virtual void Clock() override;

  virtual void DoQuarterFrame() override;
  virtual void DoHalfFrame() override;

  void SetEnabled(bool enabled);

  void SetRegister0(uint8_t data);
  void SetRegister2(uint8_t data);
  void SetRegister3(uint8_t data);

  uint8_t Output();
  bool Status();

private:
  bool        m_enabled = false;
  bool        m_controlFlag = false;

  Sequencer     m_sequencer;
  LengthCounter m_lengthCounter;
  LinearCounter m_linearCounter;

  static const std::vector<uint8_t> m_sequenceValues;
};