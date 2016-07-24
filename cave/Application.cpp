#include "Application.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include "Utils.h"
#include "Room.h"

void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const  void* userParam)
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


bool Application::init(SDL_Window* window)
{
	m_win = window;
	while (glGetError() != GL_NO_ERROR);

#if _DEBUG
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

	const char vShaderStr[] =
		"attribute vec4 v_position; \n"
		"uniform mat4 vp_matrix; \n"
		"varying vec3 pos;"
		"void main() \n"
		"{ \n"
		" pos = (vp_matrix * v_position).xyz;"
		" gl_Position = vp_matrix * v_position; \n"
		"} \n";

	const char fShaderStr[] =
		"precision mediump float; \n"
		"varying vec3 pos;"
		"void main() \n"
		"{ \n"
		//" gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0); \n"
		" gl_FragColor = vec4(pos, 1.0); \n"
		"} \n";

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	m_program = std::make_unique<ShaderProgram>(vShaderStr, fShaderStr);

	GLfloat vertices[][3] =
	{ 
		{ -1.f, -1.f, 0.0f },
		{ 1.f, -1.f, 1.0f },
		{ 0.f, 1.f, 2.0f },

		{ -1.f, 0.f, 3.0f },
		{ 1.f, 0.f, 4.0f },
		{ 0.f, 2.f, 5.0f },

		{ -1.f, 2.f, -6.0f },
		{ 1.f, 2.f, -7.0f },
		{ 0.f, 4.f, -8.0f },

		{ -1.f, 1.f, -9.0f },
		{ 1.f, 1.f, -10.0f },
		{ 0.f, 3.f, -11.0f }
	};

	static const GLfloat vertices_cube[][3] = {
		{ -1.0f, -1.0f, -1.0f }, // triangle 1 : begin
		{ -1.0f, -1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f }, // triangle 1 : end
		{ 1.0f, 1.0f, -1.0f }, // triangle 2 : begin
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f }, // triangle 2 : end
		{ 1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f }
	};//36

	
	static VertexBuffer vb(36, c_defaultVf);
	static IndexBuffer ib(36);
	m_test.m_vertices = &vb;
	m_test.m_indices = &ib;

	glm::vec3 base;

	for (int i = 0; i < 36; i++)
	{
		const auto& tri = vertices_cube[i];
		//const auto& tri = vertices[i];
		glm::vec3 pos(tri[0], tri[1], tri[2]);
		//printf("{%f,%f,%f}\n", pos.x, pos.y, pos.z);
		vb.value<glm::vec3, VertexAttributeIndex::Pos>(i) = pos;
		ib.intPointer()[i] = i;

		m_debugDraw.addVec(pos, glm::normalize(pos));
		//m_debugDraw.addVec(pos, pos);
	}
	 
	Room::generate(m_debugDraw);

	m_test.setupVbo();
	m_debugDraw.m_element.setupVbo();

	//SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(0);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	return true;
}

void Application::render()
{
	//printf("Application::render\n");
	glm::mat4 model_matrix;

	// Setup
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderElement(*m_program, m_test);

	glDisable(GL_DEPTH_TEST);
	renderElement(m_debugDraw.m_program, m_debugDraw.m_element, 0, m_debugDraw.m_currentIdx);
	glEnable(GL_DEPTH_TEST);

	SDL_GL_SwapWindow(m_win);
}

void Application::renderElement(const ShaderProgram& program, const RenderElement& element, int begin, int count)
{
	if (count == 0)
		return;

	program.bind();
	program.setVpMatrix(m_camera.getVpMatrix());
	element.render(begin, count);
}

void Application::updateMouseView(int x, int y)
{
	//printf("Application::updateMouseView\n");
	const auto mid = m_windowSize * .5f;
	const auto move = (mid - glm::vec2(x, y)) * c_mouseSpeed * m_dt * 100.f;
	m_camera.rotateBy(move.x, move.y);
	
	SDL_WarpMouseInWindow(m_win, (int)mid.x, (int)mid.y);
}

void Application::update(float dt)
{
	//printf("Application::update");
	m_dt = dt;
	m_input.UpdateControlls();
	const float move = m_dt * c_speed;
	m_camera.moveForward(move * m_input.getControlls().forwad);
	m_camera.strafe(move * m_input.getControlls().strafe);
	//printf(" forward = %f\n", m_input.getControlls().forwad);
}

void Application::reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	m_windowSize = glm::vec2((float)width, (float)height);
	m_camera.setProjection(glm::vec2(width, height));
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