#include "pch.h"
#include "ShaderProgram.h"
#include "VertexFormat.h"


ShaderProgram::ShaderProgram()
{
}


ShaderProgram::~ShaderProgram()
{

}

void ShaderProgram::bind() const
{
	glUseProgram(m_program);
	CheckGlError();
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
	
	m_vmMatrixLoc = glGetUniformLocation(m_program, "v_mMatrix");
	assert(m_vmMatrixLoc != GL_INVALID_VALUE);

	m_vvMatrixLoc = glGetUniformLocation(m_program, "v_vMatrix");
	assert(m_vvMatrixLoc != GL_INVALID_VALUE);

	m_vpMatrixLoc = glGetUniformLocation(m_program, "v_pMatrix");
	assert(m_vpMatrixLoc != GL_INVALID_VALUE);


	m_Texture0Loc = glGetUniformLocation(m_program, "texture0");
	assert(m_Texture0Loc != GL_INVALID_VALUE);

	m_vLightPos0Loc = glGetUniformLocation(m_program, "v_lightPos");

	return true;
}

void ShaderProgram::setPMatrix(const glm::mat4& matrix) const
{
	glUniformMatrix4fv(m_vpMatrixLoc, 1, GL_FALSE, glm::value_ptr(matrix));
	CheckGlError();
}

void ShaderProgram::setMMatrix(const glm::mat4& matrix) const
{
	glUniformMatrix4fv(m_vmMatrixLoc, 1, GL_FALSE, glm::value_ptr(matrix));
	CheckGlError();
}

void ShaderProgram::setVMatrix(const glm::mat4& matrix) const
{
	glUniformMatrix4fv(m_vvMatrixLoc, 1, GL_FALSE, glm::value_ptr(matrix));
	CheckGlError();
}

void ShaderProgram::setTexture(int texture) const
{
	glUniform1i(m_Texture0Loc, texture);
	CheckGlError();
}

void ShaderProgram::setLightPos(const glm::vec3& pos) const
{
	if (m_vLightPos0Loc == GL_INVALID_VALUE)
	{
		return;
	}

	glUniform3fv(m_vLightPos0Loc, 1, glm::value_ptr(pos));
	CheckGlError();
}