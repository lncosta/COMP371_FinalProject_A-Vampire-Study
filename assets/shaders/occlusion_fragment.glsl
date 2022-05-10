#version 330 core


out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec4 Color; 
in float UseTexture;
in vec4 FragPosLightSpace;

void main(){
	FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f); 
}