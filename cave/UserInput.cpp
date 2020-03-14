#include "pch.h"
#include "UserInput.h"

void UserInput::keyDown(SDL_Keycode key)
{
	m_states[key] = 1;
	//printf("Pressed %i, state = %i\n", key, m_states[key]);
}

void UserInput::keyUp(SDL_Keycode key)
{
	m_states[key] = 0;
	//printf("Deressed %i, state = %i\n", key, m_states[key]);
}

void UserInput::UpdateControlls()
{
	if (m_states[SDLK_w])
		m_controlls.forwad = 1;
	else if (m_states[SDLK_s])
		m_controlls.forwad = -1;
	else
		m_controlls.forwad = 0;

	if (m_states[SDLK_d])
		m_controlls.strafe = 1;
	else if (m_states[SDLK_a])
		m_controlls.strafe = -1;
	else
		m_controlls.strafe = 0;
}

const Controlls& UserInput::getControlls() const
{
	return m_controlls;
}