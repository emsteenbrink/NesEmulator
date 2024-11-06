
#include "NesEmulator.h"
#include <SDL.h>
#include <chrono>


extern uint8_t g_overrrun;
extern uint8_t g_underrun;

int main(int /*argc*/, char* /*argv*/[])
{
  //long long residualTime = 0;
  //std::cout << "Start with: " << argc << argv;
  NesEmulator nesEmulator;
  nesEmulator.Run();

//  auto last = std::chrono::high_resolution_clock::now();
//  auto now = std::chrono::high_resolution_clock::now();
//  long long increment = 1000000000;
//
//  while (true)
//  {
//    now = std::chrono::high_resolution_clock::now();
//    auto elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last).count();
//    last = now;
//    if (residualTime > 0)
//      residualTime -= elapsedTime;
//    else
//    {
//      residualTime += increment - elapsedTime;
//      std::cout << (g_overrrun > 0 ? 1 : g_underrun > 0 ? -1 : 0) << std::endl;
//    }
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
//  }
////  nesEmulator.Construct(780, 480, 2, 2);
////  nesEmulator.Start();
  return 0;
}