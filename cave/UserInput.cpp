#include "pch.h"
#include "UserInput.h"

void UserInput::keyDown(SDL_Keycode key)
{
	m_states[key] = 1;
	//printf("Pressed %i, state = %i\n", key, m_states[key]);
}

void UserInput::keyUp(SDL_Keycode key)
{
	m_states.erase(key);
	m_released.insert(key);
	//printf("Deressed %i, state = %i\n", key, m_states[key]);
}

void UserInput::UpdateControlls()
{
	m_released.clear();

	if (m_states[SDLK_w])
		m_controlls.forward = 1;
	else if (m_states[SDLK_s])
		m_controlls.forward = -1;
	else
		m_controlls.forward = 0;

	if (m_states[SDLK_d])
		m_controlls.strafe = 1;
	else if (m_states[SDLK_a])
		m_controlls.strafe = -1;
	else
		m_controlls.strafe = 0;
}

bool UserInput::isKeyPressed(SDL_Keycode key) const
{
	return m_states.find(key) != end(m_states);
}

bool UserInput::isKeyReleased(SDL_Keycode key) const
{
	return m_released.find(key) != end(m_released);
}

const Controlls& UserInput::getControlls() const
{
	return m_controlls;
}