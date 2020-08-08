#include "pch.h"
#include "ShaderProgram.h"
#include "VertexFormat.h"
#include "FsUtils.h"


ShaderProgram::ShaderProgram()
	:Resource()
{

}


ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_program);
	glDeleteShader(_vertexShader);
	glDeleteShader(_fragmentShader);
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
			//printf("Error compiling shader:\n%s\n", infoLog);
			LOG("Error compiling shader:");
			dbg_puts(infoLog);
			dbg_puts("\n");
			delete[] infoLog;
		}
		glDeleteShader(shader);
		return -1;
	}
	return shader;
}

bool ShaderProgram::init(const char* vertex, const char* fragment)
{
	//GLuint vertexShader;
	//GLuint fragmentShader;
	GLuint programObject;
	GLint linked;

	// Load the vertex/fragment shaders
	_vertexShader = LoadShader(GL_VERTEX_SHADER, (const char*)vertex);
	_fragmentShader = LoadShader(GL_FRAGMENT_SHADER, (const char*)fragment);

	programObject = glCreateProgram();
	if (programObject == 0)
		return false;

	glAttachShader(programObject, _vertexShader);
	glAttachShader(programObject, _fragmentShader);
	
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

	m_vlMatrix = glGetUniformLocation(m_program, "v_lMatrix");

	m_Texture0Loc = glGetUniformLocation(m_program, "texture0");
	assert(m_Texture0Loc != GL_INVALID_VALUE);

	m_vLightPos0Loc = glGetUniformLocation(m_program, "v_lightPos");

	m_TextureShadowLoc = glGetUniformLocation(m_program, "textureShadow");

	glUseProgram(m_program);
	setTexture(0);
	setTextureShadow(1);
	glUseProgram(0);

	return true;
}

bool ShaderProgram::build(const char* filename)
{
	addFileDep(filename);

	std::string dir;
	getDirectory(filename, dir);

	std::string file;
	getFileName(filename, file);

	FILE* f = fopen(filename, "r");
	if (!f)
	{
		return false;
	}
	char line[513];
	char buffer[256] = { 0 };
	std::string header;
	std::string vertex;
	std::string fragment;
	std::string includeBody;
	std::string includeFile;
	std::string* section = &header;
	int n = 0;

	std::vector<fileStackEntry_t> stack;
	fileStackEntry_t entry;
	entry.file = f;
	strcpy(entry.filename, filename);
	stack.push_back(entry);

	while (getLine(line, stack))
	{
		if (line[0] != '#')
		{
			*section += line;
			continue;
		}

		if (sscanf(line, "#include \"%[^\"]", buffer) == 1)
		{
			*section += "//";
			*section += line;

			includeFile.clear();
			getDirectory(stack.back().filename, includeFile);
			includeFile += "/";
			includeFile += buffer;

			//readFile(includeFile.c_str(), includeBody);
			//*section += includeBody;
			//*section += "\n";
			FILE* inc = fopen(includeFile.c_str(), "r");
			if (inc == NULL)
			{
				LOG("Failed to open include file %s", includeFile.c_str());
			}
			addFileDep(includeFile.c_str());
			fileStackEntry_t entry;
			entry.file = inc;
			strcpy(entry.filename, includeFile.c_str());
			stack.push_back(entry);
			continue;
		}
		else if (sscanf(line, "#pragma %s", buffer) == 1)
		{
			if (strcmp(buffer, "vertex") == 0)
			{
				section = &vertex;
			} 
			else if (strcmp(buffer, "fragment") == 0)
			{
				section = &fragment;
			}
			
			continue;
		}
		else
		{
			*section += line;
		}
	}
	header += "//end of header\n";
	std::string vertexBody = header;
	vertexBody.append(vertex);
	std::string fragmentBody = header;
	fragmentBody.append(fragment);

	const char* buildTemp = "..\\build\\shaders\\";
	std::string vertexFilename = buildTemp;
	vertexFilename += file;
	vertexFilename += "_v.glsl";
	ensureDir(vertexFilename.c_str());
	FILE* vertexFile = fopen(vertexFilename.c_str(), "w");
	if (!vertexFile)
	{
		return false;
	}
	fprintf(vertexFile, "%s\n", "#define vertex");
	fprintf(vertexFile, "%s", vertexBody.c_str());
	fclose(vertexFile);

	std::string fragmentFilename = buildTemp;
	fragmentFilename += file;
	fragmentFilename += "_f.glsl";
	FILE* fragmentFile = fopen(fragmentFilename.c_str(), "w");
	if (!fragmentFile)
	{
		return false;
	}

	fprintf(fragmentFile, "%s\n", "#define fragment");
	fprintf(fragmentFile, "%s", fragmentBody.c_str());
	fclose(fragmentFile);
	return init(vertexBody.c_str(), fragmentBody.c_str());
}

bool ShaderProgram::init(const char* filename)
{
	bool ret = build(filename);
	if (!ret)
	{
		return false;
	}

	ret = Resource::init(filename);
	if (!ret)
	{
		return false;
	}

	std::string file;
	getFileName(filename, file);
	glObjectLabel(GL_PROGRAM, m_program, -1, file.c_str());

	return true;
}

void ShaderProgram::refresh()
{
#if defined(BUILD_TOOLS)
	glDeleteProgram(m_program);
	glDeleteShader(_vertexShader);
	glDeleteShader(_fragmentShader);
	clearDeps();
	build(_name.c_str());
#endif	
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

void ShaderProgram::setLMatrix(const glm::mat4& matrix) const
{
	assert(m_vlMatrix != GL_INVALID_VALUE);
	glUniformMatrix4fv(m_vlMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
	CheckGlError();
}

void ShaderProgram::setTexture(int texture) const
{
	if (m_Texture0Loc == GL_INVALID_VALUE)
	{
		return;
	}
	glUniform1i(m_Texture0Loc, texture);
	CheckGlError();
}

void ShaderProgram::setTextureShadow(int texture) const
{
	if (m_TextureShadowLoc == GL_INVALID_VALUE)
	{
		return;
	}

	glUniform1i(m_TextureShadowLoc, texture);
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

