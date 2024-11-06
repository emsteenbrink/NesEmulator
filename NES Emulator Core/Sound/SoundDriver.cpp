#include "SoundDriver.h"


std::vector<SoundDriver*> SoundDriver::m_instances;

SoundDriver::SoundDriver(SampleGenerator& sampleGenerator, unsigned int nSampleRate, int16_t nChannels, unsigned int nBlocks, unsigned int nBlockSamples)
  : m_sampleGenerator(sampleGenerator)
{
  m_instances.push_back(this);
  Create(m_instances.size() - 1, nSampleRate, nChannels, nBlocks, nBlockSamples);
}

SoundDriver::~SoundDriver()
{
  m_bReady = false;
  if (m_thread.joinable())
    m_thread.join();
}

void SoundDriver::Create(size_t instanceId, unsigned int nSampleRate, int16_t nChannels, unsigned int nBlocks, unsigned int nBlockSamples)
{
  m_bReady = false;
  m_nSampleRate = nSampleRate;
  m_nChannels = nChannels;
  m_nBlockCount = nBlocks;
  m_nBlockSamples = nBlockSamples;
  m_nBlockFree = m_nBlockCount;
  m_nBlockCurrent = 0;
  m_pBlockMemory = nullptr;
  m_pWaveHeaders = nullptr;

  WAVEFORMATEX waveFormat;
  waveFormat.wFormatTag = WAVE_FORMAT_PCM;
  waveFormat.nSamplesPerSec = m_nSampleRate;
  waveFormat.wBitsPerSample = 16;
  waveFormat.nChannels = m_nChannels;
  waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
  waveFormat.cbSize = 0;

  // Open Device if valid
  if (waveOutOpen(&m_hwDevice, 0, &waveFormat, (DWORD_PTR)WaveOutProcWrap, instanceId, CALLBACK_FUNCTION) != S_OK)
    throw std::exception("SoundDriver::Create: Failed to waveOutOpen");

  // Allocate Wave|Block Memory
  m_pBlockMemory = new int16_t[(long long)m_nBlockCount * m_nBlockSamples];
  memset(m_pBlockMemory, 0, (size_t)2 * m_nBlockCount * m_nBlockSamples);

  m_pWaveHeaders = new WAVEHDR[m_nBlockCount];
  memset(m_pWaveHeaders, 0, sizeof(WAVEHDR) * m_nBlockCount);

  // Link headers to block memory
  for (unsigned int n = 0; n < m_nBlockCount; n++)
  {
    m_pWaveHeaders[n].dwBufferLength = m_nBlockSamples * 2;
    m_pWaveHeaders[n].lpData = (LPSTR)(&m_pBlockMemory[n * m_nBlockSamples]);
  }

  m_bReady = true;

  m_thread = thread(&SoundDriver::MainThread, this);

  // Start the ball rolling
  unique_lock<mutex> lm(m_muxBlockNotZero);
  m_cvBlockNotZero.notify_one();
}

// Handler for soundcard request for more data
inline void SoundDriver::WaveOutProc(HWAVEOUT /*hWaveOut*/, UINT uMsg, DWORD /*dwParam1*/, DWORD /*dwParam2*/)
{
  if (uMsg != WOM_DONE) return;

  m_nBlockFree++;
  unique_lock<mutex> lm(m_muxBlockNotZero);
  m_cvBlockNotZero.notify_one();
}

// Static wrapper for sound card handler
inline void SoundDriver::WaveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
  m_instances[dwInstance]->WaveOutProc(hWaveOut, uMsg, dwParam1, dwParam2);
}

// Main thread. This loop responds to requests from the soundcard to fill 'blocks'
// with audio data. If no requests are available it goes dormant until the sound
// card is ready for more data. The block is fille by the "user" in some manner
// and then issued to the soundcard.

inline void SoundDriver::MainThread()
{
  m_dGlobalTime = 0.0;
  double dTimeStep = 1.0 / (double)m_nSampleRate;

  auto tp1 = std::chrono::system_clock::now();
  auto tp2 = std::chrono::system_clock::now();

  while (m_bReady)
  {
    // Wait for block to become available
    if (m_nBlockFree == 0)
    {
      unique_lock<mutex> lm(m_muxBlockNotZero);
      m_cvBlockNotZero.wait(lm);
    }

    // Block is here, so use it
    m_nBlockFree--;

    // Prepare block for processing
    if (m_pWaveHeaders[m_nBlockCurrent].dwFlags & WHDR_PREPARED)
      waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));

    int nCurrentBlock = m_nBlockCurrent * m_nBlockSamples;

    tp2 = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedTime = tp2 - tp1;
    tp1 = tp2;

//    float fElapsedTime = elapsedTime.count();

    m_sampleGenerator.FillSamples(&m_pBlockMemory[nCurrentBlock], m_nBlockSamples, m_dGlobalTime, dTimeStep);

    m_dGlobalTime += dTimeStep * (float)m_nBlockSamples;

    // Send block to sound device
    waveOutPrepareHeader(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));
    waveOutWrite(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));
    m_nBlockCurrent++;
    m_nBlockCurrent %= m_nBlockCount;
  }
}
