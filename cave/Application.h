#pragma once

#include <memory>

#include "GlmIncludes.h"
#include <SDL2/SDL.h>

#include "Camera.h"
#include "render/RenderElement.h"
#include "render/Renderer.h"
#include "UserInput.h"
#include "render/ShaderProgram.h"
#include "render/DebugDraw.h"


const float c_speed = 300.0f;
const float c_mouseSpeed = 0.005f;

namespace ecs3
{
	class World;
};

class Application
{
public:
	virtual ~Application() {}
	bool init(SDL_Window* window);
	virtual void subInit() {}
	void render();
	void update(float dt);
	virtual void subUpdate() {};
	void finish();
	void reshape(int width, int height);
	
	glm::vec2 getWindowSize() { return m_windowSize; }
	void updateMouseView(int dx, int dy);
	void keyEvent(SDL_Keycode key, bool pressed);
	UserInput& getInput() { return m_input; }
    static Application* get();
    static Renderer* getRenderer();

private:
	void updateMouseView();
	

private:
	float m_dt = 0;
	glm::vec2 m_windowSize;
    static Application* _app;
	
	int m_lastX = 0;
	int m_lastY = 0;
	SDL_Window *m_win;
	UserInput m_input;

protected:
    Renderer _renderer;
	ecs3::World* _world;
};

