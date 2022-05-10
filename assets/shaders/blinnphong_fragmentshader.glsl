#version 330 core


//Interpolated from vertex shader:
in vec3 position;
in vec3 normal;
in vec3 eyedirection;  
in vec3 lightdirection; 
in vec3 viewposition; 

//Constant attributes:
uniform float ka;   // Ambient reflection coefficient
uniform float kd;   // Diffuse reflection coefficient
uniform float ks;   // Specular reflection coefficient
uniform float pc;   // Phong coefficient

// Material color - ambient, diffuse, and specular
uniform vec3 ac;
uniform vec3 dc;
uniform vec3 sc;

uniform mat4 modelview;  
uniform vec3 modelview_lightPosition;

out vec3 color; 

void main(){
	vec3 N = normalize(normal);
	vec3 L = normalize(modelview_lightPosition - position);
	vec3 V = normalize(viewposition - position); 

	vec3 H = normalize(L + V);
	float l = max(dot(N, L), 0.0);
	float spec = 0.0;

	if(l > 0.0){
		spec = pow(max(dot(N, H), 0.0), pc);
	}

	color =  vec3(ka * ac +
                      kd * l * dc +
                      ks * spec * sc);
	

}
