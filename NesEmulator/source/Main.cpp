#include "NesEmulator.h"

#include <SDL.h>
#include <chrono>
#include <memory>

int main(int argc, char* argv[])
{
  auto nesEmulator = std::make_unique<NesEmulator>();
  nesEmulator->Run();
  return 0;
}