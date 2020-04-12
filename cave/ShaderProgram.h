#pragma once

#include "GlHeaders.h"
#include "GlmIncludes.h"

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	void bind() const;
	void setPMatrix(const glm::mat4& matrix) const;
	void setMMatrix(const glm::mat4& matrix) const;
	void setVMatrix(const glm::mat4& matrix) const;
	void setTexture(int texture) const;
	void setLightPos(const glm::vec3& pos) const;
	void bindAttributes() const
	{
		bindAttributes(m_program);
	}

	bool init(const char* vertex, const char* fragment);

private:	
	static void bindAttributes(GLuint program);

private:
	GLuint m_program;
	GLuint m_vmMatrixLoc;
	GLuint m_vvMatrixLoc;
	GLuint m_vpMatrixLoc;
	GLuint m_Texture0Loc;
	GLuint m_vLightPos0Loc;
};

