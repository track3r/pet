#include "pch.h"
#include "Texture.h"
#include "Log.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

#include <stb/stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

static unsigned char mipBuffer2048[2048 * 2048 * 4] = { 0 };
static unsigned char mipBuffer1024[1024 * 1024 * 4] = { 0 };

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

void Texture::init(const char* filename)
{
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
	}

	//GL_TEXTURE_MAX_LEVEL 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipLevel);
	//CheckGlError();

	glBindTexture(GL_TEXTURE_2D, 0);
	CheckGlError();
	stbi_image_free(data);
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