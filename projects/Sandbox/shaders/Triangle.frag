#version 450 core
 
out vec4 FragColor;

in vec4 position;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;
uniform vec4 aColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;
uniform vec3 viewPos;

layout(binding = 0) uniform sampler2D diffuseMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D roughnessMap;
layout(binding = 3) uniform sampler2D metallicMap;

void main()
{
	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	//FragColor = vec4(normal, 1.0);
	//FragColor = mix(texture(albedo, texCoord), texture(albedo1, texCoord), 0.2);
	// ambient
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);

    vec3 ambient = lightColor * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - position.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * texture(diffuseMap, texCoord).xyz);
    
    // specular
    vec3 viewDir = normalize(viewPos - position.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);  
        
    vec3 result = /*ambient + */diffuse + specular;


	FragColor = /*texture(diffuseMap, texCoord) +*/ vec4(result, 1.0f);
}