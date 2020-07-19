#include "pch.h"
#include "Texture.h"
#include "Log.h"
#include "FsUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

#include <stb/stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

static thread_local unsigned char mipBuffer2048[2048 * 2048 * 4] = { 0 };
static thread_local unsigned char mipBuffer1024[1024 * 1024 * 4] = { 0 };

static unsigned char* mipBuffers[] = { mipBuffer2048, mipBuffer1024};

void CheckGlError();

Texture::Texture() 
	:m_texture(0)
	, m_size(0, 0)
{
}

Texture::Texture(Texture&& other)
{
	m_size = other.m_size;
	m_texture = other.m_texture;
	other.m_size = glm::ivec2();
	other.m_texture = 0;
}

bool loadTextureData(const char* filename, textureData_t& outData)
{
	std::string name;
	getFileName(filename, name);
	strncpy(outData.name, name.c_str(), 255);

	stbi_set_flip_vertically_on_load(1);
	int width;
	int height;
	outData.mipLevel[0].data = stbi_load(filename, &width, &height, &outData.channels, 0);
	if (outData.channels == 0 || outData.mipLevel[0].data == nullptr)
	{
		LOG("Failed: %s %s", filename, stbi_failure_reason());
		return false;
	}

	outData.mipLevel[0].width = width;
	outData.mipLevel[0].height = height;

	int mipLevel = 0;
	glm::ivec2 mipDim = glm::ivec2(width, height);

	do
	{
		unsigned char* mipBuffer = (unsigned char*)outData.mipLevel[mipLevel].data;
		
		glm::ivec2 nextDim = mipDim / 2;
		nextDim.x = nextDim.x ? nextDim.x : 1;
		nextDim.y = nextDim.y ? nextDim.y : 1;
		unsigned char* nextBuffer = (unsigned char*)malloc(outData.channels *nextDim.x * nextDim.y);
		stbir_resize_uint8(mipBuffer, mipDim.x, mipDim.y, 0, nextBuffer, nextDim.x, nextDim.y, 0, outData.channels);
		
		mipLevel++;
		mipDim = nextDim;

		outData.mipLevel[mipLevel].width = mipDim.x;
		outData.mipLevel[mipLevel].height = mipDim.y;
		outData.mipLevel[mipLevel].data = nextBuffer;
		
	} while (mipDim.x != 1 || mipDim.y != 1);

	outData.numMipLevels = mipLevel + 1;
	return true;
}

void freeTextureData(textureData_t& data)
{
	for (int i = 0; i < data.numMipLevels; i++)
	{
		if (i == 0) 
		{
			stbi_image_free(data.mipLevel[i].data);
		}
		else
		{
			free(data.mipLevel[i].data);
		}
	}
}

void Texture::init(const char* filename)
{
	std::string name;
	getFileName(filename, name);
	strncpy(_name, name.c_str(), 255);

	stbi_set_flip_vertically_on_load(1);
	int channels = 0;
	unsigned char* data = stbi_load(filename, &m_size.x, &m_size.y, &channels, 0);
	if (channels == 0)
	{
		LOG("Failed %s", filename);
		return;
	}
	glGenTextures(1, &m_texture);
	CheckGlError();

	glBindTexture(GL_TEXTURE_2D, m_texture);
	CheckGlError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	CheckGlError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	CheckGlError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	CheckGlError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CheckGlError();

	int mipLevel = 0;
	glm::ivec2 mipDim = m_size;
	GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
	while (mipDim.x != 0 && mipDim.y != 0)
	{
		unsigned char* mipBuffer = mipLevel == 0 ? data : mipBuffers[(mipLevel - 1) % 2];
		glTexImage2D(GL_TEXTURE_2D, mipLevel, format, mipDim.x, mipDim.y, 0, format, GL_UNSIGNED_BYTE, mipBuffer);
		CheckGlError();

		unsigned char* nextBuffer = mipBuffers[mipLevel % 2];
		glm::ivec2 nextDim = mipDim / 2;
		stbir_resize_uint8(mipBuffer, mipDim.x, mipDim.y, 0, nextBuffer, nextDim.x, nextDim.y, 0, channels);

		mipLevel++;
		mipDim = nextDim;
		//break;
	}

	

	//GL_TEXTURE_MAX_LEVEL 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//CheckGlError();

	glBindTexture(GL_TEXTURE_2D, 0);
	CheckGlError();
	stbi_image_free(data);
}

void Texture::init(const textureData_t& data)
{
	strncpy(_name, data.name, 255);

	glGenTextures(1, &m_texture);
	CheckGlError();

	glBindTexture(GL_TEXTURE_2D, m_texture);
	CheckGlError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	CheckGlError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	CheckGlError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	CheckGlError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CheckGlError();

	GLenum format = data.channels == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < data.numMipLevels; i++)
	{
		const mipLevel_t& mipLevel = data.mipLevel[i];

		glTexImage2D(GL_TEXTURE_2D, i, format, mipLevel.width, mipLevel.height, 0, format, GL_UNSIGNED_BYTE, mipLevel.data);
		CheckGlError();
	}

	//GL_TEXTURE_MAX_LEVEL 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//CheckGlError();

	if (data.channels == 4)
	{
		hasAlpha = true;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	CheckGlError();
}

GLuint Texture::getTexture()
{
	return m_texture;
}

const glm::vec2 Texture::size()
{
	return m_size;
}

Texture& Texture::operator=(Texture&& other)
{
	m_size = other.m_size;
	m_texture = other.m_texture;
	other.m_size = glm::ivec2();
	other.m_texture = 0;
	return *this;
}

Texture::~Texture()
{

}