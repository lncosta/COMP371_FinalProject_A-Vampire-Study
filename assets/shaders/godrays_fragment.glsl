#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;
uniform vec3 light_position;

uniform float scatter;

const int samples = 100; 

void main()
{
    if(scatter > 0){
     vec3 col = texture(screenTexture, TexCoords).rgb;
     //FragColor = vec4(col, 1.0);


    vec2 deltaTextCoord =  TexCoords - light_position.xy;
    vec2 textCord = TexCoords; 
    deltaTextCoord *= 1.0f /samples * density;

    float illuminationDecay = 1.0f;
    for(int i=0; i < samples ; i++){
        textCord -= deltaTextCoord; 
        vec3 sampled_color = texture(screenTexture, textCord).rgb;
        sampled_color *= illuminationDecay * weight;
        col += sampled_color;
        illuminationDecay *= decay; 
    }

     col *= exposure;
     FragColor = vec4(col, 1.0);
    }
    else{
     vec3 col = texture(screenTexture, TexCoords).rgb;
     FragColor = vec4(col, 1.0);


   }
   
   
    
