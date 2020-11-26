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
	char name[256];
};

bool loadTextureData(const char* filename, textureData_t& outData);
void freeTextureData(textureData_t& data);

struct textureInfo_t
{
	enum Format
	{
		Rgb8, Rgba8, Float32,
	};
	
	uint16_t width;
	uint16_t height;
	uint8_t format;
};

class Texture
{
public:
	Texture();
	Texture(Texture&& other);
	~Texture();

	void init(const char* filename);
	void init(const textureData_t& data);
	void init(const textureInfo_t& info);
	Texture& operator=(Texture&& other);
#ifdef OPENGL
	GLuint getGlObject() const { return m_texture; }
	GLenum getGlType() const { return GL_TEXTURE_2D; }
#endif
	const glm::vec2 size();

	bool hasAlpha = false;
	GLuint m_texture = 0;
	glm::ivec2 m_size = glm::ivec2(0);
	char _name[256] = {};
private:
	Texture(const Texture& o) {}
	Texture& operator=(const Texture& other)
    {
        return *this;
    }	
};

