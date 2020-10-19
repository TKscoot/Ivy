#version 450 core
 
out vec4 FragColor;


in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

layout(binding = 0) uniform sampler2D diffuseMap;
layout(binding = 1) uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int useNormalMap;

void main()
{           
	vec3 normal = Normal;
	
	if(useNormalMap == 1)
	{
		 // obtain normal from normal map in range [0,1]
		normal = texture(normalMap, TexCoords).rgb;
		// transform normal vector to range [-1,1]
		normal = normal * 2.0 -1.0;
		normal = normalize(TBN * normal);
	}

	// diffuse color
	vec3 color = texture(diffuseMap, TexCoords).rgb;
	// ambient
	vec3 ambient = .1 * color;
	// diffuse lighting
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;
	// specular lighting
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = vec3(0.2) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
