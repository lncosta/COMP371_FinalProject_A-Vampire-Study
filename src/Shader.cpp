#include "Shader.h"


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  





Shader::Shader(string vertex, string fragment)
{

	//Reading vertex and fragment source codes:

	string temp_vertexCode;
	string temp_fragmentCode;
	string line;

	stringstream vertexLines; 
	stringstream fragmentLines; 
	ifstream vertexFile; 
	ifstream fragmentFile;
	
	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		//Read file lines into streams, then convert into single string:
		
		vertexFile.open(vertex);
		cout << "Vertex shader opened." << endl;
		vertexLines << vertexFile.rdbuf(); 
		vertexFile.close();
		temp_vertexCode = vertexLines.str();

		fragmentFile.open(fragment);
		cout << "Fragment shader opened." << endl;
		fragmentLines << fragmentFile.rdbuf(); 
		fragmentFile.close();
		temp_fragmentCode = fragmentLines.str(); 
		
	}
	catch (const std::exception& ex) {
		cout << "Error in reading shader file." << endl;
		cout << ex.what() << endl; 
	}
	catch (...) {
		cout << "Error in reading shader file." << endl; 
	}

	//Convert into proper format for OpenGL:

	const char* vertexCode = temp_vertexCode.c_str();
	const char* fragmentCode = temp_fragmentCode.c_str(); 

	//Compile the shaders:

	unsigned int shaderVertex;
	unsigned int shaderFragment;
	unsigned int shaderProgram;

	//Vertex shader:
	shaderVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shaderVertex, 1, &vertexCode, NULL);
	glCompileShader(shaderVertex);

	
	//Check for errors in shader creation:
	char log[1024];
	int  result;
	glGetShaderiv(shaderVertex, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(shaderVertex, 1024, NULL, log);
		cout << "Error: failed shader compilation.\n" << log << endl;
	}

	//Fragment shader:
	shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shaderFragment, 1, &fragmentCode, NULL);
	glCompileShader(shaderFragment);

	glGetShaderiv(shaderFragment, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(shaderFragment, 1024, NULL, log);
		cout << "Error: failed shader compilation.\n" << log << endl;
	}

	//Creation of shader program object:
	id = glCreateProgram();

	glAttachShader(id, shaderVertex);
	glAttachShader(id, shaderFragment);
	glLinkProgram(id);

	//Check for errors:

	glGetProgramiv(id, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(id, 1024, NULL, log);
		cout << "Error: failed shader linkage.\n" << log << endl;
	}

	//Deleting the superfluous shaders:

	glDeleteShader(shaderVertex);
	glDeleteShader(shaderFragment); 

	

}

void Shader::use()
{
	glUseProgram(id); 
}

//Uniform setter functions:

void Shader::setFloat(const string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setBool(const string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (bool) value);
}

void Shader::setInt(const string& name, int value) const

{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(id,name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}




