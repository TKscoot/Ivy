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
layout(binding = 8) uniform sampler2D shadowDepthMap;

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

const float PI = 3.14159265359;

// functions
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 sRGB_linear(vec3 rgb)
{
    return pow(rgb, vec3(2.2));
}

vec3 linear_sRGB(vec3 linear)
{
    return pow(linear, vec3(1.0/2.2));
}

float ShadowCalculation(vec4 fragPosLightSpace, float NdL, sampler2D shadowMap)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow

    float bias = max(0.05 * (1.0 - NdL), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}


void main()
{           
	vec3 N = Normal;
	
	if(useNormalMap == 1)
	{
		 // obtain normal from normal map in range [0,1]
		N = texture(normalMap, TexCoords).rgb;
		// transform normal vector to range [-1,1]
		N = N * 2.0 -1.0;
		N = normalize(TBN * N);
	}

	vec3  albedo	= texture(diffuseMap,   TexCoords).rgb;
	float metallic  = texture(metallicMap,	TexCoords).b;
	float roughness = texture(roughnessMap, TexCoords).g;

	vec3 WorldPos = FragPos;

	vec3 V = normalize(viewPos - WorldPos);
	vec3 R = -normalize(reflect(V, N));

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0);

	for (int i = 0; i < pointLightSize; i++) {
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].diffuse * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);
        float shadow = ShadowCalculation(FragPosLightSpace, NdotL, shadowDepthMap);

        float outerCutOff = 0.8;
        float cutOff = 0.9;

		vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float theta = dot(L, normalize(-lightDir));
        float epsilon   = cutOff - outerCutOff;
        float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

            // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow) * intensity;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }


    vec3 ambient = vec3(0.03) * albedo;


    vec3 color = ambient + Lo;
	
    FragColor = vec4(color, 1.0);
}

