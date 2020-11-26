#pragma once

#include <SDL2/SDL.h>
#include <unordered_map>
#include <unordered_set>

struct Controlls
{
	float forward = 0;
	float strafe = 0;
};

class UserInput
{
public:
	void keyDown(SDL_Keycode key);
	void keyUp(SDL_Keycode key);

	void UpdateControlls();
	const Controlls& getControlls() const;
	 
	bool isKeyPressed(SDL_Keycode key) const;
	bool isKeyReleased(SDL_Keycode key) const;
private:
	Controlls m_controlls;
	std::unordered_map<SDL_Keycode, int>  m_states;
	std::unordered_set<SDL_Keycode> m_released;
};

