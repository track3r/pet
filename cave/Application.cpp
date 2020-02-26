#include "Application.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include "Utils.h"
#include "Room.h"

#include "ECS/Ecs.h"
#include "ECS/World.h"

#include "ecs3/ecs3.h"

#if defined(WINDOWS)
void openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const  void* userParam)
{
	printf("%i - %s\n", severity, message);
	/*std::cout << "message: " << message << " ";
	std::cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
	std::cout << "ERROR";
	break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	std::cout << "DEPRECATED_BEHAVIOR";
	break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
	std::cout << "UNDEFINED_BEHAVIOR";
	break;
	case GL_DEBUG_TYPE_PORTABILITY:
	std::cout << "PORTABILITY";
	break;
	case GL_DEBUG_TYPE_PERFORMANCE:
	std::cout << "PERFORMANCE";
	break;
	case GL_DEBUG_TYPE_OTHER:
	std::cout << "OTHER";
	break;
	}

	std::cout << "id: " << id;
	std::cout << "severity: ";
	switch (severity){
	case GL_DEBUG_SEVERITY_LOW:
	std::cout << "LOW";
	break;
	case GL_DEBUG_SEVERITY_MEDIUM:
	std::cout << "MEDIUM";
	break;
	case GL_DEBUG_SEVERITY_HIGH:
	std::cout << "HIGH";
	break;
	}*/
}
#endif

bool Application::init(SDL_Window* window)
{
	ecs3::Tests();
    _app = this;
	m_win = window;
    while (glGetError() != GL_NO_ERROR){}

#if defined(_DEBUG) && defined(WINDOWS)
	if (glDebugMessageCallback)
	{
		printf("Register OpenGL debug callback\n");
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
	}
	else
		printf("glDebugMessageCallback not available\n");

    
#endif

    _renderer.init();

    //SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(0);
    
    _world = new World();
    
	return true;
}

void Application::render()
{
	//printf("Application::render\n");

 
    _renderer.beginRender();
    _world->render();
    //_renderer.renderCube(glm::mat4());
    _renderer.endRender();

	SDL_GL_SwapWindow(m_win);
}

void Application::updateMouseView(int x, int y)
{
	//printf("Application::updateMouseView\n");
	const auto mid = m_windowSize * .5f;
	const auto move = (mid - glm::vec2(x, y)) * c_mouseSpeed * m_dt * 100.f;
	_renderer.camera().rotateBy(move.x, move.y);
	
	SDL_WarpMouseInWindow(m_win, (int)mid.x, (int)mid.y);
}

void Application::update(float dt)
{
	//printf("Application::update");
	m_dt = dt;
	m_input.UpdateControlls();
	const float move = m_dt * c_speed;
    _renderer.camera().moveForward(move * m_input.getControlls().forwad);
    _renderer.camera().strafe(move * m_input.getControlls().strafe);
    _world->update(dt);
	//printf(" forward = %f\n", m_input.getControlls().forwad);
}

void Application::reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	m_windowSize = glm::vec2((float)width, (float)height);
    _renderer.camera().setProjection(glm::vec2(width, height));
}

void Application::keyEvent(SDL_Keycode key, bool pressed)
{
	//printf("Application::keyEvent\n");
	if (pressed)
		m_input.keyDown(key);
	else
		m_input.keyUp(key);
}

void Application::finish()
{
	glUseProgram(0);
}

Application* Application::_app = nullptr;

Application* Application::get()
{
    return Application::_app;
}

Renderer* Application::getRenderer()
{
    return &Application::_app->_renderer;
}
