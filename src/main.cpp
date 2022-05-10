/*COMP 371 Final Project
Written by Luiza Nogueira Costa
ID 40124771
Concordia University
Winter 2022
*/


#include <iostream>

//Including OpenGL Libraries:

#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  
#include "glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Camera.h"
#include "ObjectLoader.h"



using namespace std;

#define WINDOW_WIDTH 1200

#define WINDOW_HEIGHT 900

Camera camera; 

float prevx = WINDOW_WIDTH / 2.0;
float prevy = WINDOW_HEIGHT / 2.0;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int skyBoxCounter = 0;

int scatter_light = -5; 

float useShadow = -10;


float quadVertices[] = { //Quad that fills the screen for FBO use
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
};

//Debugger for OpenGL calls:

void opengl_debug(int x) {
	GLenum e = glGetError();
	if (e != GL_NO_ERROR) {
		cout << "An error was found! " << x << " " << (void*)e << endl;
	}
}
//Function for automatically adjusting the viewport when window size changes:

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Input processing:
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.processKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.processKeyboard(FOWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.processKeyboard(BACKWARD, deltaTime);
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Change light mode:
	if (action == GLFW_RELEASE) return; //only handle press events
	if (key == GLFW_KEY_Q) {
		skyBoxCounter++;
		skyBoxCounter = skyBoxCounter % 3;
	}
	else if (key == GLFW_KEY_R) {
		scatter_light *= -1;
	}
	else if (key == GLFW_KEY_Z) {
		useShadow *= -1;
	}
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() //DEBUG rendering function from LearnOpenGL
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void mouse_position_callback(GLFWwindow* window, double x, double y) {
	float xposition = static_cast<float>(x);
	float yposition = static_cast<float>(y);
	float xoff, yoff; 

	if (firstMouse) {
		prevx = xposition;
		prevy = yposition;
		firstMouse = false; 
	}
	xoff = prevx - xposition ;
	yoff = prevy - yposition;

	prevx = xposition;
	prevy = yposition; 

	camera.processMouseMovement(xoff, yoff);

}

void scroll_position_callback(GLFWwindow* window, double x, double y) {
	float yposition = static_cast<float>(y);
	camera.processMouseScroll(yposition); 
}



int main() {

	//Initializing the window:

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "VampireStudy", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Initializing GLEW:
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW could not be initialized." << std::endl;
		glfwTerminate();
		return -1;
	}

	//stbi_set_flip_vertically_on_load(true);

	//Creating the viewport:
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); //Lower left corner set to (0, 0).

	//Alert GLFW of function used for resizing the viewport when window size changes:

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Alert GLFW of mouse input:
	glfwSetCursorPosCallback(window, mouse_position_callback);
	glfwSetScrollCallback(window, scroll_position_callback); 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glfwSetKeyCallback(window, key_callback);

	//Z-Buffer:
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_LIGHTING);

	//Setting the shaders:
	Shader default;
	
	Shader fire; 

	Shader candles[5];

	Shader screen("../../../assets/shaders/screenshader_vertex.glsl", "../../../assets/shaders/screenshader_fragment.glsl");

	Shader depthShader("../../../assets/shaders/depthshader_vertex.glsl", "../../../assets/shaders/depthshader_fragment.glsl");
	
	Shader debugDepthQuad("../../../assets/shaders/depthDebug_vertex.glsl", "../../../assets/shaders/depthDebug_fragment.glsl");

	Shader occShader("../../../assets/shaders/occulsion_vertex.glsl", "../../../assets/shaders/occlusion_fragment.glsl");

	//Occlusion FBO:
	unsigned int occframebuffer;
	glGenFramebuffers(1, &occframebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, occframebuffer);

	//Color attachment texture:
	unsigned int occcolorbuffer;
	glGenTextures(1, &occcolorbuffer);
	glBindTexture(GL_TEXTURE_2D, occcolorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, occcolorbuffer, 0);


	//Depth Map FBO:
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//Framebuffer configuration:
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//Color attachment texture:
	unsigned int colorbuffer; 
	glGenTextures(1, &colorbuffer);
	glBindTexture(GL_TEXTURE_2D, colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);

	//Render Buffer Object:
	unsigned int renderbuffer;
	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "ERROR: FrameBuffer is incomplete." << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Screen VAO for Multipass rendering:
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	//Model loading:

	Model room("../../../assets/project_room_4/room.obj");
	Model sky("../../../assets/project_room_4/sky.obj");
	Model skyRed("../../../assets/project_room_4/skyRed.obj");
	Model skyNight("../../../assets/project_room_4/skyNight.obj");
	Model fireball("../../../assets/project_room_4/fireball.obj");
	Model candle[5]{
	Model("../../../assets/project_room_4/candle.obj"),
	Model("../../../assets/project_room_4/candle.obj"),
	Model("../../../assets/project_room_4/candle.obj"),
	Model("../../../assets/project_room_4/candle.obj"),
	Model("../../../assets/project_room_4/candle.obj")
	};
	
	//Light Sources:
	float spherePosition[5] = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f };
	float candle_modifier[5] = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f };
	int multiplier = 1; 
	
	//vec3 sun_position = glm::vec3(-5.0f, 3.0f, 0.0f);

	vec3 sun_position = glm::vec3(-10.0f, 8.0f, 0.0f);

	float exposure = 0.1f;
	float decay = 0.97f;
	float density = 0.5f;
	float weight = 0.25f;

	//Candle Positions:

	vec3 candle_position[5] = {vec3(1, 0.75, 0),vec3(-1, 1.5, 0.25), vec3(-1, 1, -2), vec3(-1.25, 1, 0.25), vec3(-2, 1.25, -1)};

	//Skybox:

	Shader skyShader("../../../assets/shaders/cubemap_vertex.glsl", "../../../assets/shaders/cubemap_fragment.glsl");
	

	//Render loop:

	while (!glfwWindowShouldClose(window)) //If window has not been closed...
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window); //Processing inpout for loop iteration. 

		//Candlelight
		for (int i = 0; i < 5; i++) {
			
			if (i % 2 == 0) {
				spherePosition[i] = (sin(glfwGetTime() * 0.25)) * 0.25;
			}
			else {
				spherePosition[i] = -(sin(glfwGetTime() * 0.25)) * 0.25;
			}


			candle_modifier[i] = abs(sin(glfwGetTime() * 0.25 + (float)i / 5));

		}


		//Light properties:
		vec3 ambient, ambient_fire;
		vec3 color, color_fire;
		vec3 diffuse, diffuse_fire;
		vec3 specular, specular_fire;
		float scatter = -5;

		//Coloring:
		if (skyBoxCounter % 3 == 0) { //Normal
			ambient = vec3(0.50, 0.25, 0.25);
			color = vec3(1, 1, 1);
			diffuse = vec3(0.75, 0.75, 0.75);
			specular = vec3(1, 1, 1);

			ambient_fire = vec3(0.50, 0.25, 0.25);
			color_fire = vec3(1, 1, 1);
			diffuse_fire = vec3(0.75, 0.75, 0.75);
			specular_fire = vec3(1, 1, 1);

			if (scatter_light > 0) {
				scatter = 5;
			}
			else {
				scatter = -5;
			}

		}
		else if (skyBoxCounter % 3 == 1) {  //Red
			ambient = vec3(0.50, 0.25, 0.25);
			color = vec3(0.75, 0.25, 0.05);
			diffuse = vec3(0.5, 0.5, 0.5);
			specular = vec3(0.75, 0.75, 0.75);

			ambient_fire = vec3(0.50, 0.25, 0.25);
			color_fire = vec3(1, 1, 1);
			diffuse_fire = vec3(0.75, 0.75, 0.75);
			specular_fire = vec3(1, 1, 1);

			scatter = -5;
		}
		else { //Night
			ambient = vec3(0.5, 0.5, 0.5);
			color = vec3(0.2, 0.2, 1);
			diffuse = vec3(0.05, 0.05, 0.05);
			specular = vec3(0.1, 0.1, 0.1);

			ambient_fire = vec3(0.50, 0.25, 0.25);
			color_fire = vec3(1, 1, 1);
			diffuse_fire = vec3(0.75, 0.75, 0.75);
			specular_fire = vec3(1, 1, 1);

			scatter = -5;
		}

		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT);

		vec3 lightDir = glm::vec3(0.0f, 0.0f, 0.0f) - sun_position;

		//Shadow Mapping:
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = -10.0f, far_plane = 20.0f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(sun_position, glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		//Matrix creation:
		glm::mat4 viewMatrix = camera.getViewMatrix();

		//Projection matrix:

		glm::mat4 projectionMatrix = glm::perspective(
			glm::radians(camera.zoom),
			(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
			0.1f,
			100.0f
		);

		projectionMatrix = glm::scale(projectionMatrix, glm::vec3(1.0f, -1.0f, 1.0f));

		//Loading the room:

		//Set shader uniforms:
		depthShader.setMat4("projection", projectionMatrix);
		depthShader.setMat4("view", viewMatrix);

		//Model matrix:
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.25f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

		depthShader.setMat4("model", modelMatrix);

		//Render loaded room model:

		room.draw(depthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Occlusion Pass:
		glBindFramebuffer(GL_FRAMEBUFFER, occframebuffer);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		glClearColor(1.0f,1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		occShader.use(); 


		//Set shader uniforms:
		occShader.setMat4("projection", projectionMatrix);
		occShader.setMat4("view", viewMatrix);

		//Model matrix:
		occShader.setMat4("model", modelMatrix);

		//Render loaded room model:

		room.draw(occShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Binding the FBO:
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		//Rendering commands:
		glClearColor(0.5f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Activate Shader Program:

		default.use();
		glUniform1i(glGetUniformLocation(default.id, "shadowMap"), 2);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		default.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		default.setFloat("useShadow", useShadow);

		//Matrix creation:
		viewMatrix = camera.getViewMatrix(); 

		//Projection matrix:

		projectionMatrix = glm::perspective(
			glm::radians(camera.zoom),
			(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
			0.1f,
			100.0f
		);

		projectionMatrix = glm::scale(projectionMatrix, glm::vec3(1.0f, -1.0f, 1.0f));

		//Loading the room:
	
		//Set shader uniforms:
		default.setMat4("projection", projectionMatrix);
		default.setMat4("view", viewMatrix);

		//Model matrix:
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.25f, 0.0f)); 
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		
		default.setMat4("model", modelMatrix);

		//Light properties:
		 
		
		default.setVec3("light.position", sun_position);
		default.setVec3("light.direction", lightDir);  //camera.front
		default.setVec3("viewPos", camera.position);
		default.setVec3("light.color", color);
		default.setVec3("light.ambient", ambient);
		default.setVec3("light.diffuse",diffuse);
		default.setVec3("light.specular", specular);

		
		
		//Creating the candlelight:
		
		default.setVec3("pointLights[0].position", glm::vec3(candle_position[0].x, candle_position[0].y + spherePosition[0], candle_position[0].z));
		default.setVec3("pointLights[0].ambient",ambient_fire);
		default.setVec3("pointLights[0].diffuse", diffuse_fire);
		default.setVec3("pointLights[0].specular", specular_fire);
		default.setVec3("pointLights[0].color", color_fire * candle_modifier[0]);

		default.setVec3("pointLights[1].position", glm::vec3(candle_position[1].x, candle_position[1].y + spherePosition[1], candle_position[1].z));
		default.setVec3("pointLights[1].ambient", ambient_fire);
		default.setVec3("pointLights[1].diffuse", diffuse_fire);
		default.setVec3("pointLights[1].specular", specular_fire);
		default.setVec3("pointLights[1].color", color_fire * candle_modifier[1]);

		default.setVec3("pointLights[2].position", glm::vec3(candle_position[2].x, candle_position[2].y + spherePosition[2], candle_position[2].z));
		default.setVec3("pointLights[2].ambient", ambient_fire);
		default.setVec3("pointLights[2].diffuse", diffuse_fire);
		default.setVec3("pointLights[2].specular", specular_fire);
		default.setVec3("pointLights[2].color", color_fire * candle_modifier[2]);

		default.setVec3("pointLights[3].position", glm::vec3(candle_position[3].x, candle_position[3].y + spherePosition[3], candle_position[3].z));
		default.setVec3("pointLights[3].ambient", ambient_fire);
		default.setVec3("pointLights[3].diffuse", diffuse_fire);
		default.setVec3("pointLights[3].specular", specular_fire);
		default.setVec3("pointLights[3].color", color_fire * candle_modifier[3]);

		default.setVec3("pointLights[4].position", glm::vec3(candle_position[4].x, candle_position[4].y + spherePosition[4], candle_position[4].z ));
		default.setVec3("pointLights[4].ambient", ambient_fire);
		default.setVec3("pointLights[4].diffuse", diffuse_fire);
		default.setVec3("pointLights[4].specular", specular_fire);
		default.setVec3("pointLights[4].color", color_fire * candle_modifier[4]);

		//Render loaded room model:

		room.draw(default);



		//Rendering the candles:
		glm::mat4 modelMatrixCandle[5];
		for (int i = 0; i < 5; i++) {
			candles[i].use(); 
			candles[i].setMat4("projection", projectionMatrix);
			candles[i].setMat4("view", viewMatrix);
			modelMatrixCandle[i] = glm::mat4(1.0f);
			modelMatrixCandle[i] = glm::translate(modelMatrixCandle[i], glm::vec3(candle_position[i].x, candle_position[i].y + spherePosition[i], candle_position[i].z));
			modelMatrixCandle[i] = glm::scale(modelMatrixCandle[i], glm::vec3(0.04f, 0.04f, 0.04f));
			candles[i].setMat4("model", modelMatrixCandle[i]);

			candles[i].setMat4("model", modelMatrixCandle[i]);
			candles[i].setVec3("light.position", glm::vec3(candle_position[i].x, candle_position[i].y + spherePosition[i], candle_position[i].z));
			candles[i].setVec3("light.direction", camera.front);
			candles[i].setVec3("viewPos", camera.position);
			candles[i].setVec3("light.color", vec3(1, 1, 1));
			candles[i].setVec3("light.ambient", vec3(0.75, 0.5, 0.5));
			candles[i].setVec3("light.diffuse", vec3(0.5, 0.5, 0.5)* candle_modifier[i]);
			candles[i].setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f)* candle_modifier[i]);
			candles[i].setFloat("useShadow", -100);

			candle[i].draw(candles[i]);
		}

		

		//Drawing the skybox
		
		glDepthFunc(GL_LEQUAL);
		skyShader.use();
		viewMatrix = glm::mat4(glm::mat3(camera.getViewMatrix())); //Remove the translation from view
		skyShader.setMat4("view", viewMatrix);
		skyShader.setMat4("projection", projectionMatrix);

		//Model matrix:
		glm::mat4 modelMatrix2 = glm::mat4(1.0f);
		modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(100.0f, 100.0f, 100.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, (float) glm::radians(130.0f), glm::vec3(0, 1, 0));
		skyShader.setMat4("model", modelMatrix2);
		skyShader.setFloat("useShadow", -100);
		
		if (skyBoxCounter %3 == 0) {
			sky.draw(skyShader);
		}
		else if (skyBoxCounter %3 == 1) {
			skyRed.draw(skyShader);
		}
		else {
			skyNight.draw(skyShader);
		}
		
		glDepthFunc(GL_LESS);

		//Binding back the FBO and render to screen quad:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screen.use();
		screen.setFloat("exposure", exposure);
		screen.setFloat("decay", decay);
		screen.setFloat("density", density);
		screen.setFloat("weight", weight);

		

		vec3 projected_light = glm::project(sun_position, glm::mat4(1.0f), glm::scale(projectionMatrix, glm::vec3(1.0f, -1.0f, 1.0f)), vec4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));

		vec4 clip_space_pos = projectionMatrix * viewMatrix * vec4(sun_position, 1.0);
		clip_space_pos /= clip_space_pos.w; 
		vec3 snd_space_pos = vec3(clip_space_pos.x, clip_space_pos.y, clip_space_pos.z);
		vec2 window_space_pos;
	
		window_space_pos.x = snd_space_pos.x;
		window_space_pos.y = snd_space_pos.y;
		

		screen.setVec2("light_position", window_space_pos);
		screen.setFloat("scatter", scatter);


		glBindVertexArray(quadVAO);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, occcolorbuffer);
		glUniform1i(glGetUniformLocation(screen.id, "screenTexture"), 0);

		glActiveTexture(GL_TEXTURE1);
	
		glBindTexture(GL_TEXTURE_2D, colorbuffer);
		glUniform1i(glGetUniformLocation(screen.id, "screenTexture2"), 1);

		glActiveTexture(GL_TEXTURE2);

		glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(glGetUniformLocation(screen.id, "shadowMap"), 2);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		


		//DEBUG - Depth testing
		debugDepthQuad.use();
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		//renderQuad();
		
		glfwSwapBuffers(window); // Swap the buffer for the frame and show it as an output. Double buffer is used to avoid flickering. 
		glfwPollEvents(); //Checks for events triggered.
		
	}


	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	//Terminating GLFW:

	glfwTerminate(); 
	
	return 0; 
}