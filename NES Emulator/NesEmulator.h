#pragma once

#include "Nes.h"
#include "KeyboardController.h"

#include <iostream>
#include <sstream>
#include <thread>

#include <SDL.h>


class NesEmulator
{
public:
  NesEmulator();
  virtual ~NesEmulator();

  void Run();

private:
  class MainWindowHelper : public IPixelWindow
  {
  public:
    MainWindowHelper() {};
    
    uint32_t* GetData() { return (uint32_t*) m_data; }
    // Inherited via IPixelWindow
    virtual void SetPixel(uint16_t x, uint16_t y, Color color) override
    {
      if(x < 256 && y < 240)
        m_data[y][x] = color.R << 16 | color.G << 8 | color.B;
    }

  private:
    uint32_t m_data[240][256];
  };

  void HandleEvents();
  void Update();
  void Render();

  void RunEmulator();

  bool m_windowVisible = false;
  SDL_Window* m_window = nullptr;
  SDL_Texture* m_texture = nullptr;
  SDL_Renderer* m_renderer = nullptr;
  std::string Hex(uint32_t n, uint8_t d);

  std::thread                     m_emulatorThread;
  std::atomic_bool                m_emulatorThreadActive;

  MainWindowHelper                m_mainWindow;
  Nes                             m_nes;
  KeyBoardController              m_keyboardController;
};