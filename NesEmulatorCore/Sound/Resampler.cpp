#include "Resampler.h"

#include <memory>
#include <cmath>
#include <cstring>

Resampler::Resampler(float inputRate, float outputRate)
{
  m_inputRate = inputRate;
  m_outputRate = outputRate;
  m_m = inputRate / outputRate;
  m_mf = 0.0f;
  m_outputBufIndex = 0;
  m_inputBufIndex = INPUT_BUF_LEN - 1;

  m_mf = m_m - (int)m_m;
  m_samplesRequired = (int)m_m + 2;

  m_outputBuf = new short[OUTPUT_BUF_LEN];
  m_inputBuf = new float[INPUT_BUF_LEN * 2];

  memset(m_outputBuf, 0, sizeof(m_outputBuf));

  for (int i = 0; i < INPUT_BUF_LEN * 2; i++)
    m_inputBuf[i] = 0.0f;
}

Resampler::~Resampler()
{
  delete[] m_outputBuf;
  delete[] m_inputBuf;
}

void Resampler::SetOutputRate(float outputRate)
{
  m_outputRate = outputRate;
  m_m = m_inputRate / outputRate;
}

int Resampler::GetOutputBuf(int16_t** out)
{
  *out = m_outputBuf;
  int x = m_outputBufIndex;
  m_outputBufIndex = 0;
  return x;
}

int Resampler::Available()
{
  return m_outputBufIndex;
}

void Resampler::Process(float sample)
{
  m_inputBuf[m_inputBufIndex] =
    m_inputBuf[m_inputBufIndex + INPUT_BUF_LEN] = sample;

  if (--m_samplesRequired == 0)
  {
    float y2 = m_inputBuf[m_inputBufIndex];
    float y1 = m_inputBuf[m_inputBufIndex + 1];
    float y0 = m_inputBuf[m_inputBufIndex + 2];
    float ym = m_inputBuf[m_inputBufIndex + 3];

    //4-point, 3rd-order B-spline (x-form)
    //see deip.pdf
    float ym1py1 = ym + y1;
    float c0 = 1.0f / 6.0f * ym1py1 + 2.0f / 3.0f * y0;
    float c1 = 1.0f / 2.0f * (y1 - ym);
    float c2 = 1.0f / 2.0f * ym1py1 - y0;
    float c3 = 1.0f / 2.0f * (y0 - y1) + 1.0f / 6.0f * (y2 - ym);
    float j = ((c3 * m_mf + c2) * m_mf + c1) * m_mf + c0;

    float extra = 2.0f - m_mf;
    float n = m_m - extra;
    m_mf = n - (int)n;
    m_samplesRequired = (int)n + 2;
    static const float max_out = 32767.0f;
    int s = (int)round(j * max_out);

    if (s > 32767)
      s = 32767;
    else if (s < -32768)
      s = -32768;
    m_outputBuf[m_outputBufIndex++] = (short)s;
  }
  m_inputBufIndex = (m_inputBufIndex - 1) & 0x3;
}
