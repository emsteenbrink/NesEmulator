#include "BlockBuffer.h"

#include <cassert>
#include <cstring>

BlockBuffer::BlockBuffer(uint64_t inputRate, uint64_t outputRate)
  : m_resampler((float)inputRate, (float)outputRate)
  , m_readIndex(0)
  , m_writeIndex(0)
  , m_buffers(new std::array<std::array<uint8_t, BLOCK_SIZE>, BLOCK_COUNT>())
{
}

BlockBuffer::~BlockBuffer()
{
  delete m_buffers;
}
//std::atomic<float> g_currentRate = 0;

uint8_t g_overrrun = 0;

void BlockBuffer::AddSample(int16_t sample)
{
  if (!CanIncrementWriteIndex())
  {
    g_overrrun = 20;
    return;
  }

  if(g_overrrun)g_overrrun--;
  
  m_resampler.Process((float)sample / INT16_MAX);
  
  if (m_resampler.Available() == BLOCK_SIZE/2)
  {
    int16_t* buff;
    m_resampler.GetOutputBuf(&buff);
    memcpy((int16_t *)(*m_buffers)[m_writeIndex].data(), buff, BLOCK_SIZE);
    IncrementWriteIndex();

    int avail = CalcAvailable();
    auto maxDelta = 0.005;
    double fillLevel = ((double)BLOCK_COUNT - avail)/BLOCK_COUNT;
    double dynamicFrequency = ((1.0 - maxDelta) + 2.0 * fillLevel * maxDelta) * 44100.0;
    //g_currentRate = (float)dynamicFrequency;
    m_resampler.SetOutputRate((float)dynamicFrequency);
  }
}

uint8_t g_underrun = 0;

void BlockBuffer::FillSamples(uint8_t* buffer, size_t size)
{
  assert(size == BLOCK_SIZE);

  if (!CanIncrementReadIndex())
  {
    g_underrun = 20;
    memset(buffer, 0, size);
    return;
  }
  if(g_underrun) g_underrun--;
  memcpy(buffer, (*m_buffers)[m_readIndex].data(), size);
  IncrementReadIndex();
}

bool BlockBuffer::CanIncrementReadIndex()
{
  if (m_readIndex < m_writeIndex)
  {
    return m_writeIndex - m_readIndex > 1;
  }
  else
  {
    return m_writeIndex != 0 || (m_readIndex != BLOCK_COUNT-1);
  }
}

bool BlockBuffer::CanIncrementWriteIndex()
{
  if (m_writeIndex < m_readIndex)
  {
    return m_readIndex - m_writeIndex > 1;
  }
  else
  {
    return m_readIndex != 0 || m_writeIndex != (BLOCK_COUNT - 1);
  }
}

void BlockBuffer::IncrementReadIndex()
{
  m_readIndex = (m_readIndex + 1) % BLOCK_COUNT;
}

void BlockBuffer::IncrementWriteIndex()
{
  m_writeIndex = (m_writeIndex + 1) % BLOCK_COUNT;
}

//std::atomic<uint16_t> g_currentAvailable = 0;

uint16_t BlockBuffer::CalcAvailable()
{
  uint16_t readCnt = m_readIndex;
  uint16_t writeCnt = m_writeIndex;

  if (readCnt < writeCnt)
  {
   // g_currentAvailable = writeCnt - readCnt;
    return writeCnt - readCnt;
  }
  else
  {
  //  g_currentAvailable = BLOCK_COUNT - (readCnt - writeCnt);
    return BLOCK_COUNT - (readCnt - writeCnt);
  }
}
