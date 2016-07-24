#pragma once

#include <SDL2\SDL.h>
#include <unordered_map>

struct Controlls
{
	float forwad = 0;
	float strafe = 0;
};

class UserInput
{
public:
	void keyDown(SDL_Keycode key);
	void keyUp(SDL_Keycode key);

	void UpdateControlls();
	const Controlls& getControlls() const;

private:
	Controlls m_controlls;
	std::unordered_map<SDL_Keycode, int>  m_states;
};

