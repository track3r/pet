#pragma once

#include "GlHeaders.h"

#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp> 

class ShaderProgram
{
public:
	ShaderProgram(const char* vertex, const char* fragment);
	~ShaderProgram();

	void bind() const;
	void setVpMatrix(const glm::mat4& matrix) const;
	void bindAttributes() const
	{
		bindAttributes(m_program);
	}

private:
	bool init(const char* vertex, const char* fragment);
	static void bindAttributes(GLuint program);

private:
	GLuint m_program;
	GLuint m_vpMatrixLoc;
};

