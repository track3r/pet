#include "Texture.h"

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

Texture::Texture(const char* filename) 
	:m_texture(0)
	, m_size(0, 0)
{
	int n = 4;
	unsigned char* data = stbi_load(filename, &m_size.x, &m_size.y, &n, 4);
	glGenTextures(1, &m_texture);
	CheckGlError();
	glBindTexture(GL_TEXTURE_2D, m_texture);
	CheckGlError();
	int mipLevel = 0;
	glm::ivec2 mipDim = m_size;
	while (mipDim.x != 0 && mipDim.y != 0)
	{
		unsigned char* mipBuffer = mipLevel == 0 ? data : mipBuffers[(mipLevel - 1) % 2];
		glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, mipDim.x, mipDim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, mipBuffer);
		CheckGlError();
		
		unsigned char* nextBuffer = mipBuffers[mipLevel % 2];
		glm::ivec2 nextDim = mipDim / 2;
		stbir_resize_uint8(mipBuffer, mipDim.x, mipDim.y, 0, nextBuffer, nextDim.x, nextDim.y, 0, 4);

		mipLevel++;
		mipDim = nextDim;
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	CheckGlError();
	stbi_image_free(data);
}

Texture::Texture(Texture&& other)
{
	m_size = other.m_size;
	m_texture = other.m_texture;
	other.m_size = glm::ivec2();
	other.m_texture = 0;
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