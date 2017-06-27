#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "constants.hpp"
#include <iostream>


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	GLfloat Yaw;
	GLfloat Pitch;
	// Camera options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = -115.0f, GLfloat pitch = -28.0f) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(5.0f), MouseSensitivity(0.25f), Zoom(45.0f)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(5.0f), MouseSensitivity(0.25f), Zoom(45.0f)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->WorldUp);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
		if(direction == FORWARD)
			this->Position += this->Front * velocity;
		if(direction == BACKWARD)
			this->Position -= this->Front * velocity;
		if(direction == LEFT)
			this->Position -= this->Right * velocity;
		if(direction == RIGHT)
			this->Position += this->Right * velocity;
		//std::cout << Position.x << "  " << Position.y  << "  " << Position.z << std::endl;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if(constrainPitch)
		{
			if(this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if(this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}
		//std::cout << Yaw << "pitch " << Pitch << std::endl;
		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();
	}

	void set(glm::vec3 pos , GLfloat yaw, GLfloat pitch) {
		this->Position = pos;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		this->updateCameraVectors();
	}
	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if(this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if(this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if(this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}

	void StaticCamera() {
		this->MouseSensitivity = 0;
	}

	// http://stackoverflow.com/questions/29997209/opengl-c-mouse-ray-picking-glmunproject
	glm::vec3 create_ray(float xpos, float ypos) const
	{
		// these positions must be in range [-1, 1] (!!!), not [0, width] and [0, height]
		auto mouseX = xpos / (c::width  * 0.5f) - 1.0f;
		auto mouseY = ypos / (c::height * 0.5f) - 1.0f;

		glm::mat4 projection = glm::perspective(this->Zoom, c::aspectRatio, 0.1f, 1000.0f);
		glm::mat4 view = this->GetViewMatrix();

		glm::mat4 inverseVP = glm::inverse(projection * view);
		glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
		glm::vec4 worldPos = inverseVP * screenPos;

		glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

		return dir;
	}

private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	}
};