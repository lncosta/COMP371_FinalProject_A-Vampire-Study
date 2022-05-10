#version 330 core


out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D screenTexture2;
uniform sampler2D shadowMap; 

uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;
uniform vec2 light_position;

uniform float scatter;

const int samples = 128; 

void main()
{
    if(scatter > 0){
     vec3 col = vec3(0, 0, 0);
     //FragColor = vec4(col, 1.0);


    vec2 deltaTextCoord = vec2(TexCoords.xy - light_position.xy);
    vec2 textCord = TexCoords.xy; 
    deltaTextCoord *=(1.0 /  float(samples))  * density;

    float illuminationDecay = 1.0f;
    for(int i=0; i < samples ; i++){
        textCord -= deltaTextCoord; 
        vec3 sampled_color = texture(screenTexture, textCord).rgb;
        sampled_color *= illuminationDecay * weight;
        col += sampled_color;
        illuminationDecay *= decay; 
    }

     col *= exposure;

     col *= vec3(1.0f, 0.5f, 0); 
     
     vec3 col2 = texture(screenTexture2, TexCoords).rgb;

     vec3 blend = (col + col2); 

     blend = pow(blend,vec3(1.0 /2) ); 

     blend.r = clamp(blend.r, 0.0, 1.0);
     blend.g = clamp(blend.g, 0.0, 1.0);
     blend.b = clamp(blend.b, 0.0, 1.0);

     FragColor = vec4(blend, 1.0);
    }
    else{
     vec3 col = texture(screenTexture2, TexCoords).rgb;
     FragColor = vec4(col, 1.0);


   }
   
   
    
} 