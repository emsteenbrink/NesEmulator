#include "SoundUtils/BlockBuffer.h"

#include <SDL.h>

class SoundController
{
public:
  SoundController();
  virtual ~SoundController();

  // Callback function from SDL2
  void FillSamples(uint8_t* buf, size_t size);
  ISoundSampleProcessor& GetSoundSampleProcessor() { return m_blockBuffer; };

private:
  static void my_audio_callback(void* userdata, Uint8* stream, int len);

  BlockBuffer m_blockBuffer;
};