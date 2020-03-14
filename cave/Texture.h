#pragma once

#include "GlHeaders.h"

struct mipLevel_t
{
	void* data;
	uint16_t width;
	uint16_t height;
};

const int c_maxMipLevels = 16;
struct textureData_t
{
	mipLevel_t mipLevel[c_maxMipLevels];
	int channels;
	int numMipLevels;
};

bool loadTextureData(const char* filename, textureData_t& outData);
void freeTextureData(textureData_t& data);

class Texture
{
public:
	Texture();
	Texture(Texture&& other);
	~Texture();

	void init(const char* filename);
	void init(const textureData_t& data);

	Texture& operator=(Texture&& other);
	GLuint getTexture();
	const glm::vec2 size();

private:
	Texture(const Texture& o) {}
	Texture& operator=(const Texture& other)
    {
        return *this;
    }

private:
	GLuint m_texture = 0;
	glm::ivec2 m_size;
};

