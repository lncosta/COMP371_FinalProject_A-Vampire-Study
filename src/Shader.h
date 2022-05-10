#pragma once

#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

//Based off the standard Shader class definition from LearnOpengGL
class Shader {
public:

	unsigned int id;

	Shader() : Shader("../../../assets/shaders/basevertex.glsl", "../../../assets/shaders/basefragment.glsl") {}; //Default shader

	Shader(string vertex, string fragment);

	void use();

	//Utility functions:
	void setFloat(const string& name, float value) const;

	void setBool(const string& name, bool value) const;

	void setInt(const string& name, int value) const;

	void setVec2(const std::string& name, const glm::vec2& value) const;

	void setVec3(const std::string& name, const glm::vec3& value) const;

	void setVec4(const std::string& name, const glm::vec4& value) const;

	void setMat2(const std::string& name, const glm::mat2& mat) const;

	void setMat3(const std::string& name, const glm::mat3& mat) const;

	void setMat4(const std::string& name, const glm::mat4& mat) const;
	


};