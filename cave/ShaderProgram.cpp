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

	m_vlMatrix = glGetUniformLocation(m_program, "v_lMatrix");

	m_Texture0Loc = glGetUniformLocation(m_program, "texture0");
	assert(m_Texture0Loc != GL_INVALID_VALUE);

	m_vLightPos0Loc = glGetUniformLocation(m_program, "v_lightPos");

	m_TextureShadowLoc = glGetUniformLocation(m_program, "textureShadow");

	return true;
}
int splitPath(const char* fullpath, int& filename)
{
	size_t l = strlen(fullpath);
	if (l == 0)
	{
		return -1;
	}

	do
	{
		char c = fullpath[l - 1];
		if (c == '\\' || c == '/')
		{
			return (int)l;
		}
	} while (--l > 0);

	return (int)l;
}

void getFileName(const char* fullpath, std::string& result)
{
	size_t l = strlen(fullpath);
	if (l == 0)
	{
		result = "";
		return;
	}
	do
	{
		char c = fullpath[l - 1];
		if (c == '\\' || c =='/')
		{
			result.assign(fullpath + l);
			return;
		}
	} while (--l > 0);

	result = fullpath;
}

void getDirectory(const char* fullpath, std::string& result)
{
	size_t l = strlen(fullpath);
	if (l == 0)
	{
		result = "";
		return;
	}

	do
	{
		char c = fullpath[l - 1];
		if (c == '\\' || c == '/')
		{
			result.assign(fullpath, l);
			return;
		}
	} while (--l > 0);

	result = fullpath;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>

bool ensureDir(const char* path)
{
	char temp[512] = { 0 };
	size_t l = strlen(path);
	if (l == 0)
	{
		return true;
	}

	int i = 0;
	int begin = 0;
	bool foundFile = false;
	do
	{
		
		if (path[i] == '/' || path[i] == '\\')
		{
			if (foundFile)
			{
				LOG("%s: dir in the middle", path);
				return false;
			}
			struct stat info;

			if (stat(temp, &info) != 0)
			{
				if (_mkdir(temp) != 0)
				{
					LOG("failed to make dir %s", temp);
					return false;
				}
			}
			else if (info.st_mode & S_IFDIR)
			{
				
			} 
			else
			{
				foundFile = true;
			}
		}

		temp[i] = path[i];
	} while (++i < l);

	return true;
}

bool readFile(const char* filename, std::string& result)
{
	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		return false;
	}

	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	result.resize(len);
	fseek(f, 0, SEEK_SET);
	fread(&result.front(), 1, len, f);

	return true;
}

bool getLine(char* line, std::vector<FILE*>& stack)
{
	if (stack.empty())
	{
		return false;
	}

	while (fgets(line, 512, stack.back()) == NULL)
	{
		fclose(stack.back());
		stack.pop_back();

		if (stack.empty())
		{
			return false;
		}
	}

	return true;
}

bool ShaderProgram::init(const char* filename)
{
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

	std::vector<FILE*> stack;
	stack.push_back(f);

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

			includeFile = dir;
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
			stack.push_back(inc);
			continue;
		}

		if (sscanf(line, "#pragma %s", buffer) == 1)
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

	fprintf(fragmentFile, "%s", fragmentBody.c_str());
	fclose(fragmentFile);

	return init(vertexBody.c_str(), fragmentBody.c_str());
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

