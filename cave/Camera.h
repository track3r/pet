#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

class Camera
{
public:
	Camera();

	void setProjection(glm::vec2 view, float fov = 45);
	
	glm::mat4 getVpMatrix() const;

	void rotateBy(float horizontal, float vertical);
	void moveForward(float distance);
	void strafe(float distance);

private:
	void computeView();

private:
	glm::mat4 m_projection;
	glm::mat4 m_view;
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	glm::vec3 m_direction = glm::vec3(0, 0, -1);
	glm::vec3 m_up = glm::vec3(0, 1, 0);
	glm::vec3 m_right = glm::vec3(0, 0, 1);

	// horizontal angle : toward -Z
	float m_horizontalAngle = 3.14f;

	// vertical angle : 0, look at the horizon
	float m_verticalAngle = 0.0f;
	
	// Initial Field of View
	float m_fov = 45.0f;
};

