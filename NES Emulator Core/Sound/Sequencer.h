#pragma once

#include <cstdint>
#include <vector>

class Sequencer
{
public:
  Sequencer(uint8_t extraTicks);

  void SetEnabled(bool enabled);

  void SetSequence(const std::vector<uint8_t>* sequence);
  void SetTimerLow(uint8_t timerLow);
  void SetTimerHigh(uint8_t timerHigh);

  uint8_t GetOutput();

  void Clock();
 
  // Used by the sweep to adjust the timer;
  uint16_t TimerLoad();
  void TimerLoad(uint16_t timerLoad);

private:
  uint8_t                     m_extraTicks = 0;
  bool                        m_enabled = false;
  const std::vector<uint8_t>* m_sequence = nullptr;
  uint16_t                    m_timerLoad = 0x0000;

  uint16_t                    m_timer = 0x0000;
  uint8_t                     m_currentSequenceIndex = 0;
};
