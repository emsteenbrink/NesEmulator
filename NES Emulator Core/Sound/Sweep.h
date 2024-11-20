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
  Sequencer&    m_sequencer;
  bool          m_add1sComplement;

  bool          m_enabled = false;
  uint8_t       m_dividerPeriod = 0;
  bool          m_negateFlag = false;
  uint8_t       m_shiftCount = 0;

  bool          m_reload = false;
  uint8_t       m_divider = 0;
};