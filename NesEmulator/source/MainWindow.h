#pragma once

#include "IPixelWindow.h"

#include "Components/PPU_2C02.h"

#include <SDL.h>

class MainWindow : public IPixelWindow
{
public:
  MainWindow(uint16_t width, uint16_t height);

  virtual void SetPixel(uint16_t x, uint16_t y, Color color) override;

  void Render();

private:
  SDL_Window*             m_window = nullptr;
  SDL_Texture*            m_texture = nullptr;
  SDL_Renderer*           m_renderer = nullptr;

  uint16_t                m_width;
  uint16_t                m_height;
  std::vector<uint32_t>   m_data;
};