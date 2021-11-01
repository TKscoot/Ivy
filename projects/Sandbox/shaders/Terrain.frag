
#version 450 core
 
out vec4 FragColor;

layout(binding = 0) uniform sampler2D diffuseMap;
layout(binding = 1) uniform sampler2D redMap;

in vec3 FragPos;
in vec3 ViewPosition;
in vec2 TexCoords;
in vec3 Normal;

uniform vec3 dirLightDirection;
uniform float steepnessCutoff;


const vec3 Ca = vec3(  0.00005, 0.0000492, 0.0000439);
const vec3 Cd = vec3(  1.0,  1.0, 1.0);
const vec3 Cs = vec3 (.8, .8, .8);
const vec3 Ce = vec3 ( 0,  0,  0);

// Direktionales Licht (im Unendlichen)
const vec3 light = normalize (vec3 (2, 1, 3));

void main()
{        

	vec3 diffTex = texture(diffuseMap, TexCoords * 5.0).rgb;
	vec3 redTex = texture(redMap, TexCoords * 5.0).rgb;

    vec3 norm = normalize(Normal);

    // Blending between textures based on steepness
    float slope = 1 - norm.y;
    float grassBlendHeight = steepnessCutoff * (1 - 0.5);
    float grassWeight = 1 - clamp((slope-grassBlendHeight)/(steepnessCutoff-grassBlendHeight), 0.0, 1.0);

    vec3 color = diffTex * grassWeight + redTex * (1-grassWeight);

	// ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * Cd.xyz;
  	
    // diffuse 
    vec3 lightDir = normalize(dirLightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * Cd.xyz;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(ViewPosition - FragPos.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * Cd.xyz;  
        
    vec3 result = (ambient + diffuse + specular) * color;

    FragColor = vec4(vec3(result), 1.0);
}
		