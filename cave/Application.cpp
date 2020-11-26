#include "pch.h"
#include "Application.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include "Log.h"
#include "Utils.h"

#include "render/RenderWorld.h"

#include "ecs3/SampleRenderSystem.h"
#include "ecs3/PlayerSystem.h"
#include "ecs3/MeshRenderSystem.h"
#include "ecs3/TransformAnimationSystem.h"

#include "Resource.h"

#if defined(_MSC_VER) 
char g_bufferInfo[512] = {};
void openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const  void* userParam)
{
	if (source == GL_DEBUG_SOURCE_APPLICATION)
	{
		return;
	}

	const char signature[] = "Buffer info:";
	if (strncmp(message, signature, sizeof(signature) - 1) == 0)
	{
		strcpy_s(g_bufferInfo, message);
		return;
	}
	LOG_NOFILELINE("%i - %s", severity, message);
	assert(type != GL_DEBUG_TYPE_ERROR);
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
	//ecs3::Tests();
    TRACE();
    _app = this;
	m_win = window;
    while (glGetError() != GL_NO_ERROR){}

#if defined(_DEBUG) && defined(_MSC_VER)
	if (glDebugMessageCallback)
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE,	GL_DONT_CARE, GL_DONT_CARE,	0, NULL, GL_TRUE);
		glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_FALSE);
	}
#endif
    _renderer.init();

    //SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(0);
    
	_world = new ecs3::World();
	_world->registerSystem<ecs3::InputSystem>();
	_world->registerSystem<TransformAnimationSystem>();
	_world->registerSystem<PlayerSystem>();

	_world->registerSystem<ecs3::SampleSystem>();
	_world->registerSystem<ecs3::SampleRenderSystem>();
	
	_world->registerSystem<LightRenderSystem>();
	_world->registerSystem<MeshRenderSystem>();

	_world->get<ecs3::InputSingleton>().mouseSpeed = c_mouseSpeed;

	RenderWorld* renderWorld = new RenderWorld();
	if (!renderWorld->init())
	{
		LOG("RenderWorld init failed");
		return false;
	}
	_world->get<RenderSingleton>().world = renderWorld;

	subInit();
	return true;
}

void Application::render()
{
    _renderer.beginRender();
	ViewMatrices viewParms;
	viewParms.projection = _renderer.camera().getProjection();
	viewParms.view = _renderer.camera().getView();
	_world->get<RenderSingleton>().world->render(viewParms);
    _renderer.endRender();

	SDL_GL_SwapWindow(m_win);
}

static int firstUpdates = 10;
void Application::updateMouseView(int x, int y)
{
	//printf("Application::updateMouseView\n");
	glm::vec2 mid = m_windowSize * .5f;
	glm::vec2 move = (mid - glm::vec2(x, y)) * c_mouseSpeed * m_dt * 100.f;
	if (firstUpdates > 0)
	{
		firstUpdates--;
		SDL_WarpMouseInWindow(m_win, (int)mid.x, (int)mid.y);
	}
	//printf("x: %i, y: %i\n", x, y);
	
	_world->get<ecs3::InputSingleton>().mouse = mid - glm::vec2(x, y);
	_renderer.camera().rotateBy(move.x, move.y);
	
	SDL_WarpMouseInWindow(m_win, (int)mid.x, (int)mid.y);
}

void Application::update(float dt)
{
	//printf("Application::update");
	m_dt = dt;
	m_input.UpdateControlls();

	ecs3::FrameSingleton& frameInfo = _world->get<ecs3::FrameSingleton>();
	frameInfo.dt = dt;
	frameInfo.number++;

	if (frameInfo.number % 100)
	{
		resourceManager->checkForUpdates();
	}

	const float move = m_dt * c_speed;
    _renderer.camera().moveForward(move * m_input.getControlls().forward);
    _renderer.camera().strafe(move * m_input.getControlls().strafe);
	//_renderer.beginRender();
    _world->update();
	//_renderer.endRender();
	//printf(" forward = %f\n", m_input.getControlls().forward);
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
	{
		m_input.keyDown(key);
		_world->get<ecs3::InputSingleton>().keyDown(key);
	}
	else
	{
		if (key == SDLK_F1)
		{
			_renderer.camera().toggleDebug();
		}
		m_input.keyUp(key);
		_world->get<ecs3::InputSingleton>().keyUp(key);
	}
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
