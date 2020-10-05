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

// calculates the color when using a directional light.
vec3 CalcDirLight(Material light, vec4 diffuseTexture, vec3 direction, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient *			vec3(diffuseTexture);
    vec3 diffuse = light.diffuse * diff *	vec3(diffuseTexture);
    vec3 specular = light.specular * spec * vec3(diffuseTexture);
    return (ambient + diffuse + specular);
}

void main()
{
	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	//FragColor = vec4(normal, 1.0);
	//FragColor = mix(texture(albedo, texCoord), texture(albedo1, texCoord), 0.2);
	// ambient
	vec4 diffTex = texture(diffuseMap, texCoord);

	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 lightPos = vec3(20.2f, 1.0f, 2.0f);

    vec3 ambient = lightColor * material.ambient;
  	
    vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - position.xyz);

    // 
	vec3 result = CalcDirLight(material, diffTex, vec3(-0.2, -1.0, -0.3), norm, viewDir);

    vec3 lightDir = normalize(lightPos - position.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * diffTex.xyz);
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);  
        
    result += /*ambient + */diffuse + specular;


	FragColor = vec4(result, diffTex.a);
}