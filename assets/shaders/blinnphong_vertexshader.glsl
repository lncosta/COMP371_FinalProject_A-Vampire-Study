#version 330 core

//Vertex input data:
layout(location = 0) in vec3 modelview_vertexPosition;
layout(location = 1) in vec3 modelview_vertexNormal;

//Output data:

out vec3 position;
out vec3 normal;
out vec3 eyedirection;
out vec3 lightdirection; 
out vec3 viewposition; 

//modelview = MVP

uniform mat4 modelview, v;
uniform vec3 modelview_lightPosition; 
uniform vec3 modelview_viewPosition; 

void main(){
	
	gl_Position = modelview * vec4(modelview_vertexPosition, 1); 

	position = (vec4(modelview_vertexPosition, 1)).xyz;
	
	vec3 cameraspace_vertexPosition = (v * vec4(modelview_vertexPosition, 1)).xyz;

	eyedirection = modelview_viewPosition - cameraspace_vertexPosition;

	vec3 cameraspace_lightPosition = (v * vec4(modelview_lightPosition, 0)).xyz;

	lightdirection = cameraspace_lightPosition + eyedirection; 

	normal = (v * vec4(modelview_vertexNormal, 0)).xyz; 

	viewposition = modelview_viewPosition; 

}
