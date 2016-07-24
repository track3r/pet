#pragma once

#include "GlHeaders.h"
#include <glm/glm.hpp>

class Texture
{
public:
	Texture()
		:m_texture(0)
		, m_size(0, 0)
	{

	}

	~Texture()
	{

	}

	Texture& operator=(Texture&& other)
	{
		m_size = other.m_size;
		m_texture = other.m_texture;
		other.m_size = glm::ivec2();
		other.m_texture = 0;
	}

	Texture(Texture&& other)
	{
		m_size = other.m_size;
		m_texture = other.m_texture;
		other.m_size = glm::ivec2();
		other.m_texture = 0;
	}

	GLuint getTexture()
	{
		return m_texture;
	}

	const glm::vec2 size() 
	{ 
		return m_size; 
	}

private:
	Texture(const Texture& o) {}
	Texture& operator=(const Texture& other) {}

private:
	GLuint m_texture;
	glm::ivec2 m_size;
};

