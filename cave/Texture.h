#pragma once

#include "GlHeaders.h"
#include <glm/glm.hpp>

class Texture
{
public:
	Texture(const char* filename);

	~Texture();

	Texture& operator=(Texture&& other);

	Texture(Texture&& other);

	GLuint getTexture();

	const glm::vec2 size();

private:
	Texture(const Texture& o) {}
	Texture& operator=(const Texture& other) {}

private:
	GLuint m_texture;
	glm::ivec2 m_size;
};

