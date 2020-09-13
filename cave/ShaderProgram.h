#pragma once

#include "GlHeaders.h"
#include "GlmIncludes.h"

class ShaderProgram : public Resource
{
public:
	ShaderProgram();
	~ShaderProgram();

	void setPMatrix(const glm::mat4& matrix) const;
	void setMMatrix(const glm::mat4& matrix) const;
	void setVMatrix(const glm::mat4& matrix) const;
	void setLMatrix(const glm::mat4& matrix) const;
	void setDrawId(uint32_t drawData) const;
	void setTexture(int texture) const;
	void setTextureShadow(int texture) const;
	void setLightPos(const glm::vec3& pos) const;
	void bindAttributes() const
	{
		bindAttributes(m_program);
	}

	bool build(const char* filename);
	bool init(const char* vertex, const char* fragment);
	bool init(const char* filename);
	void clear();
	void refresh();
#ifdef OPENGL
	GLuint getGlProgram() const { return m_program; };
#endif
private:	
	static void bindAttributes(GLuint program);

private:
	GLuint m_program = -1;
	GLuint _vertexShader = -1;
	GLuint _fragmentShader = -1;
	GLuint m_vmMatrixLoc;
	GLuint m_vvMatrixLoc;
	GLuint m_vpMatrixLoc;
	GLuint m_Texture0Loc;
	GLuint m_TextureShadowLoc;
	GLuint m_vLightPos0Loc;
	GLuint m_vlMatrix;
	GLuint _drawIdLoc = -1;
};

