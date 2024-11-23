#include "Envelope.h"

Envelope::Envelope()
{
}

void Envelope::Loop(bool loop)
{
  m_loop = loop;
}

void Envelope::UseConstantVolume(bool useConstantVolume)
{
  m_useConstantVolume = useConstantVolume;
}

void Envelope::EnvelopeParameter(uint8_t parameter)
{
  m_envelopeParameter = parameter;
}

void Envelope::Restart()
{
  m_restart = true;
}

void Envelope::Clock()
{
  if (m_restart)
  {
    m_restart = false;
    m_decayLevel = 15;
    m_divider = m_envelopeParameter;
  }
  else
  {
    if (m_divider == 0)
    {
      m_divider = m_envelopeParameter;
      if (m_decayLevel == 0)
      {
        if (m_loop)
        {
          m_decayLevel = 15;
        }
      }
      else
      {
        m_decayLevel--;
      }
    }
    else
    {
      m_divider--;
    }
  }
}

#include <atomic>
std::atomic<uint8_t>  g_decayLevel = 0;
std::atomic_bool g_useConstant = false;
uint8_t Envelope::GetOutput()
{
  //g_decayLevel = m_decayLevel;
  //g_useConstant = m_useConstantVolume;
  return m_useConstantVolume ? m_envelopeParameter : m_decayLevel;
}
