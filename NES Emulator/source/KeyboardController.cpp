#include "KeyboardController.h"

#include <iostream>

using namespace std;

KeyBoardController::KeyBoardController()
{
  if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
  {
    cout << "SDL could not initialize! SDL_Error:" << SDL_GetError() << endl;
    return;
  }

  AddAddress(0x4016);
}

void KeyBoardController::HandleEvent(const SDL_Event& event)
{
  switch (event.type)
  {
  case SDL_KEYDOWN:
    if (event.key.repeat == 0)
    {
      HandleKeyDown(event.key.keysym.sym);
    }
    break;
  case SDL_KEYUP:
    if (event.key.repeat == 0)
    {
      HandleKeyUp(event.key.keysym.sym);
    }
    break;
  case SDL_CONTROLLERBUTTONDOWN:
    HandleButtonDown((SDL_GameControllerButton)event.cbutton.button);
    break;
  case SDL_CONTROLLERBUTTONUP:
    HandleButtonUp((SDL_GameControllerButton)event.cbutton.button);
    break;
  }
}

void KeyBoardController::HandleButtonDown(SDL_GameControllerButton button)
{
  switch (button)
  {
  case SDL_CONTROLLER_BUTTON_B:                    m_controller1 |= 0x80; break;
  case SDL_CONTROLLER_BUTTON_X:                    m_controller1 |= 0x40; break;
  case SDL_CONTROLLER_BUTTON_BACK:                 m_controller1 |= 0x20; break;
  case SDL_CONTROLLER_BUTTON_START:                m_controller1 |= 0x10; break;
  case SDL_CONTROLLER_BUTTON_DPAD_UP:              m_controller1 |= 0x08; break;
  case SDL_CONTROLLER_BUTTON_DPAD_DOWN:            m_controller1 |= 0x04; break;
  case SDL_CONTROLLER_BUTTON_DPAD_LEFT:            m_controller1 |= 0x02; break;
  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:           m_controller1 |= 0x01; break;
  }
}

void KeyBoardController::HandleButtonUp(SDL_GameControllerButton button)
{
  switch (button)
  {
  case SDL_CONTROLLER_BUTTON_B:                    m_controller1 &= ~0x80; break;
  case SDL_CONTROLLER_BUTTON_X:                    m_controller1 &= ~0x40; break;
  case SDL_CONTROLLER_BUTTON_BACK:                 m_controller1 &= ~0x20; break;
  case SDL_CONTROLLER_BUTTON_START:                m_controller1 &= ~0x10; break;
  case SDL_CONTROLLER_BUTTON_DPAD_UP:              m_controller1 &= ~0x08; break;
  case SDL_CONTROLLER_BUTTON_DPAD_DOWN:            m_controller1 &= ~0x04; break;
  case SDL_CONTROLLER_BUTTON_DPAD_LEFT:            m_controller1 &= ~0x02; break;
  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:           m_controller1 &= ~0x01; break;
  }
}

void KeyBoardController::HandleKeyUp(SDL_Keycode key)
{
  switch (key)
  {
  case SDLK_x:      m_controller1 &= ~0x80; break;
  case SDLK_z:      m_controller1 &= ~0x40; break;
  case SDLK_a:      m_controller1 &= ~0x20; break;
  case SDLK_s:      m_controller1 &= ~0x10; break;
  case SDLK_UP:     m_controller1 &= ~0x08; break;
  case SDLK_DOWN:   m_controller1 &= ~0x04; break;
  case SDLK_LEFT:   m_controller1 &= ~0x02; break;
  case SDLK_RIGHT:  m_controller1 &= ~0x01; break;
  }
}

void KeyBoardController::HandleKeyDown(SDL_Keycode key)
{
  switch (key)
  {
  case SDLK_x:      m_controller1 |= 0x80; break;
  case SDLK_z:      m_controller1 |= 0x40; break;
  case SDLK_a:      m_controller1 |= 0x20; break;
  case SDLK_s:      m_controller1 |= 0x10; break;
  case SDLK_UP:     m_controller1 |= 0x08; break;
  case SDLK_DOWN:   m_controller1 |= 0x04; break;
  case SDLK_LEFT:   m_controller1 |= 0x02; break;
  case SDLK_RIGHT:  m_controller1 |= 0x01; break;
  }
}

uint8_t KeyBoardController::Read(uint16_t address)
{
  uint8_t result = 0x00;
  switch (address)
  {
  case 0x4016:
    result = (m_controllerState1 & 0x80) > 0;
    m_controllerState1 <<= 1;
    break;
  case 0x4017:
    result = (m_controllerState2 & 0x80) > 0;
    m_controllerState2 <<= 1;
    break;
  }
  return result;
}

void KeyBoardController::Write(uint16_t address, uint8_t /*data*/)
{
  switch (address)
  {
  case 0x4016:
    m_controllerState1 = m_controller1;
    break;
  case 0x4017:
    m_controllerState2 = m_controller2;
    break;
  }
}
