#pragma once


#include <cstdint>

class Sequencer;

class Sweep
{
public:
  Sweep(Sequencer& sequencer, bool add1sComplement);

  void SetEnabled(bool enabled);
  void SetDividerPeriod(uint8_t dividerPeriod);
  void SetNegateFlag(bool negateFlag);
  void SetShiftCount(uint8_t shiftCount);

  void Clock();

private:
  Sequencer& m_sequencer;
  bool          m_add1sComplement;

  bool          m_enabled;
  uint8_t       m_dividerPeriod;
  bool          m_negateFlag;
  uint8_t       m_shiftCount;

  bool          m_reload;
  uint8_t       m_divider;
};