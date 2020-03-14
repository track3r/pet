#include "pch.h"
#include "Application.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include "Log.h"
#include "Utils.h"
#include "Room.h"

#include "ecs3/SampleRenderSystem.h"
#include "ecs3/PlayerSystem.h"
#include "ecs3/MeshRenderSystem.h"

#include "ObjReader.h"

#if defined(_MSC_VER) 
void openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const  void* userParam)
{
	LOG_NOFILELINE("%i - %s", severity, message);
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

void CreateTestComponents(ecs3::World* _world)
{
	ecs3::Configuration singleConf;
	singleConf.addComponent<ecs3::TransformComponent>();
	_world->createEntity(singleConf);

	/*ecs3::Configuration configuration;
	configuration.addComponent<ecs3::TransformComponent>();
	configuration.addComponent<ecs3::SampleComponent>();
	_world->createEntity(configuration);

	ecs3::EntitityPrefab prefab;
	prefab.addComponent(ecs3::TransformComponent(glm::vec3(1.0f, 1.0f, 1.0f)));
	prefab.addComponent(ecs3::SampleComponent());
	_world->createEntity(prefab);*/

	ecs3::EntitityPrefab playerConf;
	playerConf.addComponent(ecs3::TransformComponent(glm::vec3(10.f, 0.f, 0.f)));
	playerConf.addComponent(ecs3::SampleComponent());
	playerConf.addComponent(PlayerComponent());
	_world->createEntity(playerConf);
}

void LoadTestScene(ecs3::World* _world)
{
	ObjReader reader;
	const char* sponza = "..\\assets\\sponza\\sponza.obj";
	const char* cube = "..\\assets\\cube.obj";
	if (!reader.parse(sponza))
	{
		LOG("Failed to read %s", sponza);
		return;
	}

	ObjMtlreader mtlReader;
	const char* sponzaMtl = "..\\assets\\sponza\\sponza.mtl";
	if (!mtlReader.parse(sponzaMtl))
	{
		LOG("Failed to read %s", sponzaMtl);
		return;
	}

	std::unordered_map<std::string, Texture*> textures;
	for (const ObjMtlreader::material_t material : mtlReader.materials)
	{
		if (material.texture[0] == 0)
		{
			continue;
		}
		std::string filename = "..\\assets\\sponza\\";
		filename += material.texture;
		LOG(">>Texture %s", filename.c_str());
		Texture* texture = new Texture();
		texture->init(filename.c_str());
		textures[material.name] = texture;
		//break;
	}

	MeshComponent meshComp;
	ecs3::EntitityPrefab meshConf;
	meshConf.addComponent(ecs3::TransformComponent(glm::vec3(0.f, 0.f, 0.f)));
	meshConf.addComponent(meshComp);

	for (const ObjReader::group_t& group : reader.groups)
	{
		int faces = group.endFace - group.startFace;
		LOG(">>Mesh %s", group.name);
		VertexBuffer* vb = new VertexBuffer(faces * 3, c_defaultVf);
		IndexBuffer* ib = new IndexBuffer(faces * 3);
		uint32_t index = 0;
		for (uint32_t f = group.startFace; f < group.endFace; f++)
		{
			const ObjReader::face_t& face = reader.faces[f];
			for (int v = 0; v < 3; v++)
			{
				vb->value<glm::vec3, VertexAttributeIndex::Pos>(index) = reader.positions[face.vertices[v] - 1];
				vb->value<glm::vec2, VertexAttributeIndex::Uv>(index) = reader.texcoords[face.texcoords[v] - 1];
				ib->intPointer()[index] = index;
				index++;
			}
		}

		RenderElement* element = new RenderElement();
		element->m_indices = ib;
		element->m_vertices = vb;
		element->textures[0] = textures[group.material];
		element->setupVbo(false);
		meshComp.mesh = element;
		meshConf._data.addComponent(meshComp);
		_world->createEntity(meshConf);
		//break;
	}
}

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
    
	_world = new ecs3::World();
	_world->registerSystem<ecs3::InputSystem>();
	_world->registerSystem<PlayerSystem>();
	_world->registerSystem<ecs3::SampleSystem>();
	_world->registerSystem<ecs3::SampleRenderSystem>();
	_world->registerSystem<MeshRenderSystem>();

	_world->get<ecs3::InputSingleton>().mouseSpeed = c_mouseSpeed;
	CreateTestComponents(_world);
	LoadTestScene(_world);
	return true;
}

void Application::render()
{
	//printf("Application::render\n");

 
    //_renderer.beginRender();
    //_renderer.renderCube(glm::mat4());
    //_renderer.endRender();

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
	_world->get<ecs3::FrameSingleton>().dt = dt;
	_world->get<ecs3::FrameSingleton>().number++;

	const float move = m_dt * c_speed;
    _renderer.camera().moveForward(move * m_input.getControlls().forwad);
    _renderer.camera().strafe(move * m_input.getControlls().strafe);
	_renderer.beginRender();
    _world->update();
	_renderer.endRender();
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
