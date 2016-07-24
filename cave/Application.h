#pragma once

#include <glm\glm.hpp>
#include <SDL2\SDL.h>

#include "Camera.h"
#include "RenderElement.h"
#include "UserInput.h"
#include "ShaderProgram.h"
#include "DebugDraw.h"
#include <memory>

const float c_speed = 3.0f;
const float c_mouseSpeed = 0.005f;

class Application
{
public:

	bool init(SDL_Window* window);
	void render();
	void update(float dt);
	void finish();
	void reshape(int width, int height);
	
	void updateMouseView(int dx, int dy);
	void keyEvent(SDL_Keycode key, bool pressed);

private:
	void updateMouseView();
	void renderElement(const ShaderProgram& program, const RenderElement& element, int offset = 0, int count = -1);

private:
	float m_dt = 0;
	glm::vec2 m_windowSize;
	Camera m_camera;
	RenderElement m_test;
	std::unique_ptr<ShaderProgram> m_program;
	int m_lastX = 0;
	int m_lastY = 0;
	SDL_Window *m_win;
	UserInput m_input;
	DebugDraw m_debugDraw;
};

