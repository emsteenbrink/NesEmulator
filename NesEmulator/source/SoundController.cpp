#include "SoundController.h"

#include <iostream>
#include <cassert>

SoundController::SoundController()
  : m_blockBuffer(1789773 / 2, 44100)
{
  if (SDL_Init(SDL_INIT_AUDIO) < 0)
  {
    std::cout << "Failed to SDL_Init(SDL_INIT_AUDIO): " << SDL_GetError();
    assert(!"SDL_Init failed");
  }

  SDL_AudioSpec wanted;
  wanted.freq = 44100;
  wanted.format = AUDIO_S16;
  wanted.channels = 1;    /* 1 = mono, 2 = stereo */
  wanted.samples = 512; //512;  /* Good low-latency value for callback */
  wanted.callback = my_audio_callback;
  wanted.userdata = this;

  /* Open the audio device, forcing the desired format */
  if (SDL_OpenAudio(&wanted, nullptr) < 0)
  {
    std::cout << "Couldn't open audio: " << SDL_GetError();
    assert(!"SDL_OpenAudio failed");
  }

  SDL_PauseAudio(0);
}

SoundController::~SoundController()
{
  SDL_CloseAudio();
}

void SoundController::my_audio_callback(void* userdata, Uint8* stream, int len)
{
  SoundController* soundController = static_cast<SoundController*>(userdata);
  assert(soundController);
  if (soundController)
    soundController->FillSamples(stream, len);
}

void SoundController::FillSamples(uint8_t* buf, size_t size)
{
  m_blockBuffer.FillSamples(buf, size);
}
