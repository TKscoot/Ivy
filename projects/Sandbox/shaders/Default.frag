#version 450 core
 
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;
in vec4 FragPosLightSpace;

layout(binding = 0) uniform sampler2D diffuseMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D roughnessMap;
layout(binding = 3) uniform sampler2D metallicMap;
layout(binding = 8) uniform sampler2D shadowMap;

//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int useNormalMap;
uniform int useMetallicMap;

// Material
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

// Lights
struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

uniform int pointLightSize; // TODO: check cpu side if this is bigger than MAX_POINT_LIGHTS
uniform int spotLightSize;

uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float CalcShadow(vec4 fragPosLightSpace, vec3 lightPos, vec3 N);

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
	vec3 viewDir = normalize(viewPos - FragPos);
	
	// Calculating directional light
	vec3 result = CalcDirLight(dirLight, normal, viewDir);
	//float shadow = CalcShadow(FragPosLightSpace, normal); //TODO: Fix shadows
	//result *= shadow;
	//result += dirLight.ambient * vec3(texture(diffuseMap, TexCoords).rgb);
	
	// Calculating point lights
	for(int i = 0; i < pointLightSize; i++)
	{
		result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
	}
	
	// Calculating spot lights
	for(int i = 0; i < spotLightSize; i++)
	{
		result += CalcSpotLight(spotLights[i], normal, FragPos, viewDir);
	}

    // Gamma correction
    float gamma = 2.2;
    //result.rgb = pow(result.rgb, vec3(1.0/gamma));

    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = light.direction; //MAYBE BUGGY!! //normalize(light.direction - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(diffuseMap, TexCoords).rgb);
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, TexCoords).rgb);

	vec3 matSpec = material.specular;
	if(useMetallicMap == 1)
	{
		matSpec = vec3(texture(metallicMap, TexCoords).rgb);
	}
    vec3 specular = light.specular * spec * matSpec;

	//float shadow = CalcShadow(FragPosLightSpace, light.direction, normal);
	float NdotL = max(dot(normal, lightDir), 0.0);
	float shadow = CalcShadow(FragPosLightSpace, lightDir, normal);

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(diffuseMap, TexCoords).rgb;
    // Shadows disabled
    //return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(diffuseMap, TexCoords).rgb);
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, TexCoords).rgb);

	vec3 matSpec = material.specular;
	if(useMetallicMap == 1)
	{
		matSpec = vec3(texture(metallicMap, TexCoords).rgb);
	}
    vec3 specular = light.specular * spec * matSpec;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(diffuseMap, TexCoords).rgb);
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, TexCoords).rgb);

	vec3 matSpec = material.specular;
	if(useMetallicMap == 1)
	{
		matSpec = vec3(texture(metallicMap, TexCoords).rgb);
	}
    vec3 specular = light.specular * spec * matSpec;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

float CalcShadow(vec4 fragPosLightSpace, vec3 lightPos, vec3 N)
{

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    //projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = lightPos;//normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

