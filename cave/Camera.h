#pragma once

#include "GlmIncludes.h"

class Camera
{
public:
	Camera();

	void setProjection(glm::vec2 view, float fov = 45);
	
	glm::mat4 getVpMatrix() const;
	glm::mat4 getProjection() const { return m_projection; }
	glm::mat4 getView() const
	{
		if (_debug)
		{
			return _debugView;
		}
		else
		{
			return m_view;
		}
	}

	void rotateBy(float horizontal, float vertical);
	void moveForward(float distance);
	void strafe(float distance);

	void setView(const glm::mat4& view)
	{
		m_view = view;
	}

	void toggleDebug()
	{
		_debug = !_debug;
	}

private:
	void computeView();

private:
	glm::mat4 m_projection;
	glm::mat4 m_view;
	glm::mat4 _debugView;
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	glm::vec3 m_direction = glm::vec3(0, 0, 1);
	glm::vec3 m_up = glm::vec3(0, 1, 0);
	glm::vec3 m_right = glm::vec3(1, 0, 0);

	// horizontal angle : toward -Z
	//float m_horizontalAngle = 3.14f;
	float m_horizontalAngle = 0.f;

	// vertical angle : 0, look at the horizon
	float m_verticalAngle = 0.0f;
	
	// Initial Field of View
	float m_fov = 45.0f;

	bool _debug = true;
};

