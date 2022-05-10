#pragma once
#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <vector>

enum Movement {
	RIGHT, 
	LEFT,
	FOWARD,
	BACKWARD
};

class Camera {
public:
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 front;
	glm::vec3 worldUp;

	float yaw; 
	float pitch; 

	float sensitivity; 
	float speed;
	float zoom; 

	Camera();
	Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, glm::vec3 front, float speed, float sensitivity, float zoom);

	glm::mat4 getViewMatrix();

	void processKeyboard(Movement direction, float time);

	void processMouseMovement(float x, float y, GLboolean constraintPitch = true); 

	void processMouseScroll(float y);

	void update(); 

};

using namespace glm;
struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};