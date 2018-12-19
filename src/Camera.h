#ifndef CAMERA_H
#define CAMERA_H

#include"Renderer.h"
#include<vector>

enum class camera_movement
{
	FORWARD,BACKWARD,RIGHT,LEFT,WUP,WDOWN
};

const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ZOOM = 45.0f;

class Camera
{
private:
	glm::vec3 m_Position, m_Up, m_WorldUp, m_Front, m_Right;
	float m_Zoom, m_Pitch, m_Yaw, MovementSpeed, MouseSensitivity;
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(m_Pitch))*cos(glm::radians(m_Yaw));
		front.y = sin(glm::radians(m_Pitch));
		front.z = cos(glm::radians(m_Pitch))*sin(glm::radians(m_Yaw));
		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}
public:
	Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& WorldUp = glm::vec3(0.0f, 1.0f, 0.0f), const float& yaw = YAW, const float& pitch = PITCH)
		:MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), m_Zoom(ZOOM), m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
	{
		m_Position = position;
		m_WorldUp = WorldUp;
		m_Yaw = yaw;
		m_Pitch = pitch;
		updateCameraVectors();
	}
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}
	glm::mat4 GetLitViewMatrix(glm::vec3 frontPos)
	{
		return glm::lookAt(m_Position, frontPos, m_Up);
	}
	glm::vec3 GetPosition()
	{
		return m_Position;
	}
	glm::vec3 GetUp()
	{
		return m_Up;
	}
	glm::vec3 GetRight()
	{
		return m_Right;
	}
	glm::vec3 GetFront()
	{
		return m_Front;
	}
	float GetZoom()
	{
		return m_Zoom;
	}
	void ProcessKeyboard(const camera_movement& direction, const float& deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == camera_movement::FORWARD)
			m_Position += m_Front * velocity;
		else if (direction == camera_movement::BACKWARD)
			m_Position -= m_Front * velocity;
		else if (direction == camera_movement::RIGHT)
			m_Position += m_Right * velocity;
		else if (direction == camera_movement::LEFT)
			m_Position -= m_Right * velocity;
		else if (direction == camera_movement::WUP)
			m_Position += m_WorldUp * velocity;
		else if (direction == camera_movement::WDOWN)
			m_Position -= m_WorldUp * velocity;
	}
	void ProcessMouseMovement(float xOffset, float yOffset, bool ConstrainPitch = true)
	{
		xOffset *= MouseSensitivity;
		yOffset *= MouseSensitivity;

		m_Yaw += xOffset;
		m_Pitch += yOffset;

		if (ConstrainPitch)
		{
			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			else if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;
		}
		updateCameraVectors();
	}
	void ProcessMouseScroll(float yOffset,bool doo)
	{
		if (!doo)
			return;
		if (m_Zoom >= 1.0f && m_Zoom <= 45.0f)
			m_Zoom -= yOffset;
		if (m_Zoom <= 1.0f)
			m_Zoom = 1.0f;
		else if (m_Zoom >= 45.0f)
			m_Zoom = 45.0f;
	}
};

#endif