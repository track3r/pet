#include "Camera.h"

Camera::Camera()
{
	computeView();
}

glm::mat4 Camera::getVpMatrix() const
{
	//glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))
	//auto temp = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	//return m_projection * temp;
	if (_debug)
	{
		return m_projection * _debugView;
	}
	else
	{
		return m_projection * m_view;
	}
	
	//return m_projection * glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

void Camera::computeView()
{
	m_direction = glm::vec3(
		cos(m_verticalAngle) * sin(m_horizontalAngle),
		sin(m_verticalAngle),
		cos(m_verticalAngle) * cos(m_horizontalAngle)
		);

	m_right = glm::vec3(
		sin(m_horizontalAngle - 3.14f / 2.0f),
		0,
		cos(m_horizontalAngle - 3.14f / 2.0f)
		);

	m_up = glm::cross(m_right, m_direction);
	_debugView = glm::lookAt(m_position, m_position + m_direction, m_up);
}

void Camera::rotateBy(float horizontal, float vertical)
{
	if (fabs(horizontal) <= FLT_EPSILON && fabs(vertical) <= FLT_EPSILON)
		return;

	m_horizontalAngle += horizontal;
	m_verticalAngle += vertical;
	computeView();
}

void Camera::moveForward(float distance)
{
	m_position += distance * glm::normalize(m_direction);
	computeView();
}

void Camera::strafe(float distance)
{
	m_position += distance * glm::normalize(m_right);
}

void Camera::setProjection(glm::vec2 view, float fov)
{
	m_projection = glm::perspective(fov, view.x / view.y, 0.1f, 1000.f);
}
