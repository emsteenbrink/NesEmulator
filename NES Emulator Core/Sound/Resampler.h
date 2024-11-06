#pragma once

#include <cstdint>

class Resampler
{
public:
  Resampler(float inputRate, float outputRate);
  virtual ~Resampler();

  void SetOutputRate(float outputRate);
  void Process(float sample);
  int Available();
  int GetOutputBuf(int16_t** out);

private:
  static const int OUTPUT_BUF_LEN = 1024;
  static const int INPUT_BUF_LEN = 4;

  float                   m_inputRate;
  float                   m_outputRate;
  float                   m_m;
  float                   m_mf;
  int                     m_samplesRequired;

  int                     m_outputBufIndex;
  int                     m_inputBufIndex;
  short*                  m_outputBuf;
  float*                  m_inputBuf;
  int                     m_skip = 0;
};