#version 330 core


out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec4 Color; 
in float UseTexture;
in vec4 FragPosLightSpace;

struct Light {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 color;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

struct PointLight{
    vec3 position;  
    vec3 direction; 
    vec3 color;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};
#define NUM_POINT_LIGHTS 5

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadowMap;


uniform vec3 viewPos;
uniform Light light; //General light

uniform PointLight pointLights[NUM_POINT_LIGHTS]; //Candlelight

uniform float useShadow;

float calculateShadow(vec4 fragPosLightSpace){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float bias = max(0.0001 * (1.0 - dot(normal, light.direction)), 0.00001);
    float shadow = 0.0;

    int kernelWidth = 1; 

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -kernelWidth; x <= kernelWidth; x++){
         for(int y = -kernelWidth; y <= kernelWidth; y++){
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
         }
    }
    //shadow /= 8.0;

    shadow /= ((kernelWidth*2+1)*(kernelWidth*2+1));
    if(projCoords.z > 1.0){
        shadow = 0.1;
    }
    return shadow;
}

vec3 illuminate(PointLight point, vec3 point_color){
    
    
    if (UseTexture == 0.0f){
        //FragColor = Color;

    vec3 color = Color.xyz;

    vec3 lightDir = normalize(point.position - FragPos);

    vec3 ambient = point_color * point.ambient * color;
    
    vec3 n = normalize(Normal);
    float diff = max(dot(n, lightDir), 0);
    vec3 diffuse = point_color * point.diffuse * diff * color; 

    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 halfDir = normalize(viewDir + lightDir);
    float NH = max(dot(n, halfDir), 0);

    vec3 specular = point.specular * pow(NH, 8) * point_color * color; 

    vec3 result = ambient + diffuse + specular;
    return result; 

    }
    else {
      //FragColor = texture(texture_diffuse1, TexCoords);

    vec3 color = texture(texture_diffuse1, TexCoords).rgb;

    vec3 lightDir = normalize(point.position - FragPos);

    vec3 ambient = point_color * point.ambient * color;
    
    vec3 n = normalize(Normal);
    float diff = max(dot(n, lightDir), 0);
    vec3 diffuse = point_color * point.diffuse * diff * color; 

    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 halfDir = normalize(viewDir + lightDir);
    float NH = max(dot(n, halfDir), 0);

    vec3 specular = point.specular * pow(NH, 8) * point_color * color; 

    vec3 result = ambient + diffuse + specular;
    return result; 

    }

}
void main()
{    
    
    if (UseTexture == 0.0f){
        //FragColor = Color;

    vec3 lightDir = normalize(light.position - FragPos);

    vec3 ambient = light.ambient * Color.rgb;
    
    vec3 n = normalize(Normal);
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * Color.rgb; 

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    diffuse *= attenuation;

    vec3 result = ambient + diffuse; 
    FragColor = vec4(result, 1.0);

    }
    else {
      //FragColor = texture(texture_diffuse1, TexCoords);

    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
 
    //vec3 ambient = 0.5 * color;

    vec3 lightDir = normalize(light.position - FragPos);

    vec3 ambient = light.color * light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    
    vec3 n = normalize(Normal);
    float diff = max(dot(n, lightDir), 0);
    vec3 diffuse = light.color * light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb; 

    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 halfDir = normalize(viewDir + lightDir);
    float NH = max(dot(n, halfDir), 0);

    vec3 specular = light.specular * pow(NH, 8) * light.color * texture(texture_specular1, TexCoords).rgb; 

    float shadow = 0;
    
    if(useShadow > 0){
        shadow = calculateShadow(FragPosLightSpace);
    }
       

    vec3 result = ambient + (1 - shadow)*(diffuse + specular); 
    
    
    
    for(int i = 0; i < NUM_POINT_LIGHTS; i++){
        float distance = length(pointLights[i].position - FragPos);
        if(distance < 2.0f){
            vec3 point_color = pointLights[i].color * (2.0f - distance)/2.0f; 
            result += illuminate(pointLights[i], point_color);   
        }
       
    }  
    
    FragColor = vec4(result, 1.0);

    //FragColor = vec4(ambient, 1.0); 

   
    }

}