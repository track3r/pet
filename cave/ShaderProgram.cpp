#include "ShaderProgram.h"
#include "VertexFormat.h"
#include <stdlib.h>
#include <stdio.h>

ShaderProgram::ShaderProgram(const char* vertex, const char* fragment)
{
	init(vertex, fragment);
}


ShaderProgram::~ShaderProgram()
{

}

void ShaderProgram::bind() const
{
	glUseProgram(m_program);
}

void ShaderProgram::setVpMatrix(const glm::mat4& matrix) const
{
	glUniformMatrix4fv(m_vpMatrixLoc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::bindAttributes(GLuint program)
{
	for (const auto& atr : c_attribs)
	{
		glBindAttribLocation(program, (int)atr.index, atr.name);
	}
}

GLuint LoadShader(GLenum type, const char *shaderSrc)
{
	GLuint shader;
	GLint compiled;
	// Create the shader object
	shader = glCreateShader(type);
	if (shader == 0)
		return 0;
	// Load the shader source
	glShaderSource(shader, 1, &shaderSrc, NULL);
	// Compile the shader
	glCompileShader(shader);
	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = new char[infoLen];
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			printf("Error compiling shader:\n%s\n", infoLog);
			delete[] infoLog;
		}
		glDeleteShader(shader);
		return -1;
	}
	return shader;
}

bool ShaderProgram::init(const char* vertex, const char* fragment)
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint programObject;
	GLint linked;

	// Load the vertex/fragment shaders
	vertexShader = LoadShader(GL_VERTEX_SHADER, (const char*)vertex);
	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, (const char*)fragment);

	programObject = glCreateProgram();
	if (programObject == 0)
		return false;

	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	
	bindAttributes(programObject);

	glLinkProgram(programObject);

	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = new char[infoLen];
			glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
			delete[] infoLog;
		}
		glDeleteProgram(programObject);
		return false;
	}

	m_program = programObject;
	m_vpMatrixLoc = glGetUniformLocation(m_program, "vp_matrix");

	return true;
}
