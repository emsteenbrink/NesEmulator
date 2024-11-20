#pragma once

#include <cstdint>

class Envelope
{
public:
  Envelope();

  void Loop(bool loop);
  void UseConstantVolume(bool useConstantVolume);
  void EnvelopeParameter(uint8_t parameter);

  void Restart();

  void Clock();

  uint8_t GetOutput();

private:
  bool      m_loop = false;
  bool      m_useConstantVolume = false;
  uint8_t   m_envelopeParameter = 0x00;

  bool      m_restart = false;
  uint8_t   m_divider = 0;
  uint8_t   m_decayLevel = 15;
};