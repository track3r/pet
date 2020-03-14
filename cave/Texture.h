#pragma once

#include "GlHeaders.h"

class Texture
{
public:
	Texture();
	Texture(Texture&& other);
	~Texture();

	void init(const char* filename);

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
	GLuint m_texture;
	glm::ivec2 m_size;
};

