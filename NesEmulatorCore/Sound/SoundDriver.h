#pragma once

#pragma comment(lib, "winmm.lib")

#include <cstdint>
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>

#include <Windows.h>

using namespace std;

#undef min
#undef max

class SampleGenerator
{
public:
  virtual void FillSamples(int16_t block[], size_t size, double t, double tStep) = 0;
};

class SoundDriver
{
public:
  SoundDriver(SampleGenerator& sampleGenerator, unsigned int nSampleRate = 44100, int16_t nChannels = 1, unsigned int nBlocks = 8, unsigned int nBlockSamples = 512);
  ~SoundDriver();

private:
  void Create(size_t instanceId, unsigned int nSampleRate = 44100, int16_t nChannels = 1, unsigned int nBlocks = 8, unsigned int nBlockSamples = 512);

  // Handler for soundcard request for more data
  void WaveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwParam1, DWORD dwParam2);
  // Static wrapper for sound card handler
  static void CALLBACK WaveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

  // Main thread. This loop responds to requests from the soundcard to fill 'blocks'
  // with audio data. If no requests are available it goes dormant until the sound
  // card is ready for more data. The block is fille by the "user" in some manner
  // and then issued to the soundcard.
  void MainThread();

  unsigned int m_nSampleRate;
  uint16_t m_nChannels;
  unsigned int m_nBlockCount;
  unsigned int m_nBlockSamples;
  unsigned int m_nBlockCurrent;

  int16_t* m_pBlockMemory;
  WAVEHDR* m_pWaveHeaders;
  HWAVEOUT m_hwDevice;

  thread m_thread;
  atomic<bool> m_bReady;
  atomic<unsigned int> m_nBlockFree;
  condition_variable m_cvBlockNotZero;
  mutex m_muxBlockNotZero;

  double m_dGlobalTime;

  static std::vector<SoundDriver*> m_instances;

  SampleGenerator&  m_sampleGenerator;
};
