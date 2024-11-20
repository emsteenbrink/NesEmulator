#include "MainWindow.h"

#include <iostream>
#include <sstream>

using namespace std;

MainWindow::MainWindow(uint16_t width, uint16_t height)
  : m_width(width)
  , m_height(height)
{
  m_data.resize(m_width * m_height);

  SDL_InitSubSystem(SDL_INIT_VIDEO);
  //Create window
  m_window = SDL_CreateWindow("Nes Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width * 3, m_height * 3, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (m_window == nullptr)
  {
    cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    return;
  }

  m_renderer = SDL_CreateRenderer(m_window, -1, 0);
  if (m_renderer == nullptr)
  {
    cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
    return;
  }
  SDL_RenderSetLogicalSize(m_renderer, width, height);
  m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
}

void MainWindow::Render()
{
  extern uint8_t g_overrrun;
  extern uint8_t g_underrun;

  SDL_UpdateTexture(m_texture, nullptr, m_data.data(), m_width * sizeof(uint32_t));

  SDL_RenderClear(m_renderer);
  SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
  if (g_overrrun)
  {
    SDL_Rect rect = { .x = 10,.y = 10,.w = 5,.h = 5 };
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 0);
    SDL_RenderFillRect(m_renderer, &rect);
  }
  if (g_underrun)
  {
    SDL_Rect rect = { .x = 10,.y = 10,.w = 5,.h = 5 };
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 0);
    SDL_RenderFillRect(m_renderer, &rect);
  }
  SDL_RenderPresent(m_renderer);
}

void MainWindow::SetPixel(uint16_t x, uint16_t y, Color color)
{
  if (x < m_width && y < m_height)
    m_data[y * m_width + x] = color.R << 16 | color.G << 8 | color.B;
}
