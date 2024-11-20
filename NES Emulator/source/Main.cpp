
#include "NesEmulator.h"
#include <SDL.h>
#include <chrono>
#include <memory>

extern uint8_t g_overrrun;
extern uint8_t g_underrun;

int main(int /*argc*/, char* /*argv*/[])
{
  auto nesEmulator = std::make_unique<NesEmulator>();
  nesEmulator->Run();
  return 0;
}