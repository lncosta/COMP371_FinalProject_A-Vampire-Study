#include "Camera.h"

Camera::Camera()
{
	position = glm::vec3(0.0f, 0.20f, 0.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	yaw = -90.0f;
	pitch = 0.0f;
	front = glm::vec3(0.0f, 0.0f, -1.0f); 
	speed = 2.5f;
	sensitivity = 0.1f;
	zoom = 45.0f;  //fov
	update(); 
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, glm::vec3 front, float speed, float sensitivity, float zoom)
{

	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
	this->front = front;
	this->speed = speed;
	this->sensitivity = sensitivity;
	this->zoom = zoom;
	update(); 
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(Movement direction, float time)
{
	float vel = time * speed; 

	if (direction == RIGHT) {
		position += right * vel; 
	}
	if (direction == LEFT) {
		position -= right * vel;
	}
	if (direction == FOWARD) {
		position += front * vel;
	}
	if (direction == BACKWARD) {
		position -= front * vel;
	}

	position.y = 0.2f; //FPS camera does not allow for flying. 
}

void Camera::processMouseMovement(float x, float y, GLboolean constraintPitch)
{
	y *= sensitivity;
	x *= sensitivity;
	yaw += x;
	pitch += y;

	if (constraintPitch) {
		if (pitch < -90.0f) {
			pitch = -90.0f;
		}
		else if (pitch > 90.0f) {
			pitch = 90.0f;
		}
	}
	update(); 
}

void Camera::processMouseScroll(float y)
{
	zoom -= (float)y;

	if (zoom > 45.0f) {
		zoom = 45.0f;
	}
	else if (zoom < 1.0f) {
		zoom = 1.0f;
	}
}

void Camera::update()
{
	glm::vec3 temp_front;
	temp_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); 
	temp_front.y = sin(glm::radians(pitch));
	temp_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(temp_front);

	right = glm::normalize(glm::cross(worldUp, front));
	up = glm::normalize(glm::cross(right, front)); 
	 
}
