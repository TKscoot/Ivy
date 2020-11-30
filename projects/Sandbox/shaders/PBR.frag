#version 450 core
 
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;
in vec4 FragPosLightSpace[4];
in vec3 ViewPosition;

layout(binding = 0) uniform sampler2D   diffuseMap;
layout(binding = 1) uniform sampler2D   normalMap;
layout(binding = 2) uniform sampler2D   roughnessMap;
layout(binding = 3) uniform sampler2D   metallicMap;
layout(binding = 4) uniform sampler2D   aoMap;
layout(binding = 5) uniform sampler2D   brdfLutTexture;
layout(binding = 6) uniform samplerCube skyboxTexture;
layout(binding = 8) uniform sampler2D   shadowMap0;
layout(binding = 9) uniform sampler2D   shadowMap1;
layout(binding = 10) uniform sampler2D  shadowMap2;
layout(binding = 11) uniform sampler2D  shadowMap3;


//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool useNormalMap;
uniform bool useMetallicMap;
uniform bool useRoughnessMap;
uniform bool useIBL;

// Material
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float metallic;
	float roughness;
};

struct CascadedShadowMapParams
{
	mat4  lightView;
	bool  showCascades;
	bool  softShadows;
	float lightSize;
	float maxShadowDistance;
	float shadowFade;
	bool  cascadeFading;
	float cascadeTransitionFade;
	vec4  cascadeSplits;
};

// Lights
struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular; //-
};

struct PointLight {
    vec3 position;
    
    float constant;//-
    float linear;//-
    float quadratic;//-
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;//-
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

uniform CascadedShadowMapParams csm;

const float PI = 3.14159265359;
const float Epsilon = 0.00001;

uint  CascadeIndex = 0;
float ShadowFade   = 1.0;

// Function Prototypes
float CalcShadow(sampler2D shadowMap, vec3 fragPosLightSpace, vec3 lightPos, vec3 N);
float CalcCascadedShadows(vec3 N);
vec3  PbrDirectionalLighting(DirLight light, float roughness, float metallic, vec3 albedo, vec3 N, vec3 V, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3  fresnelSchlick(float cosTheta, vec3 F0);
float gaSchlickG1(float cosTheta, float k);
float gaSchlickGGX(float cosLi, float cosLo, float roughness);
vec3  fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

//MAIN
void main()
{		
    vec3 albedo     = pow(texture(diffuseMap, TexCoords).rgb, vec3(2.2));
    float metallic  = material.metallic;
    float roughness = material.roughness;

	vec3 N = Normal;
	if(useNormalMap)
	{
		// obtain normal from normal map in range [0,1]
		N = texture(normalMap, TexCoords).rgb;
		// transform normal vector to range [-1,1]
		N = normalize(N * 2.0 - 1.0);
		N = normalize(TBN * N);
	}
	
	//if(useMetallicMap)
	//{
		metallic = texture(metallicMap, TexCoords).r;
	//}
	
	//if(useRoughnessMap)
	//{
		roughness = texture(roughnessMap, TexCoords).r;
	//}

	vec3 V = normalize(viewPos - FragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

	vec3 radiance = vec3(1.0);

	// Directional Light
	vec3 Lo = PbrDirectionalLighting(dirLight, roughness, metallic, albedo, N, V, F0);

	// Point Lights
	for(int i = 0; i < pointLightSize; ++i) 
    {
		vec3 lightPosition = pointLights[i].position;
		vec3 lightDiffuse  = pointLights[i].diffuse;

        // calculate per-light radiance
        vec3 L = normalize(lightPosition - FragPos);
        vec3 H = normalize(V + L);
        
		float dist		  = length(lightPosition - FragPos);
        float attenuation = 1.0 / (dist * dist);
        vec3 radiance	  = lightDiffuse * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
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

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
	vec3 ambient = vec3(0);

    if (useIBL) 
	{
	    vec3 R = -normalize(reflect(V, N));

        vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 irradiance =  texture(skyboxTexture, N, 8).rgb;

        vec3 diffuse    = irradiance * albedo;

        const float MAX_REFLECTION_LOD = 7.0;
        vec3 prefilteredColor = vec3(0);

        prefilteredColor = texture(skyboxTexture, R, roughness * (MAX_REFLECTION_LOD + 1)).rgb;

        vec2 envBRDF  = texture(brdfLutTexture, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

        ambient = (kD * diffuse + specular) * dirLight.ambient;
	}
	else
	{
		ambient = vec3(0.03) * albedo;
	}

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

	if (csm.showCascades)
	{
		switch(CascadeIndex)
		{
		case 0:
			color.rgb *= vec3(1.0f, 0.25f, 0.25f);
			break;
		case 1:
			color.rgb *= vec3(0.25f, 1.0f, 0.25f);
			break;
		case 2:
			color.rgb *= vec3(0.25f, 0.25f, 1.0f);
			break;
		case 3:
			color.rgb *= vec3(1.0f, 1.0f, 0.25f);
			break;
		}
	}

	FragColor = vec4(color, 1.0);
}

float CalcShadow(sampler2D shadowMap, vec3 fragPosLightSpace, vec3 lightPos, vec3 N)
{

    // perform perspective divide
    vec3 projCoords =fragPosLightSpace;// fragPosLightSpace.xyz / fragPosLightSpace.w;
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

// Leicht abgeänderte Implementation basierend auf Yan Chernikovs Hazel Engine (https://github.com/TheCherno/Hazel)
float CalcCascadedShadows(vec3 N)
{
const uint SHADOW_MAP_CASCADE_COUNT = 4;
	for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if(ViewPosition.z < csm.cascadeSplits[i])
			CascadeIndex = i + 1;
	}

	float shadowDistance = csm.maxShadowDistance;//u_CascadeSplits[3];
	float transitionDistance = csm.shadowFade;
	float distance = length(ViewPosition);	// TODO: eventuell buggy
	ShadowFade = distance - (shadowDistance - transitionDistance);
	ShadowFade /= transitionDistance;
	ShadowFade = clamp(1.0 - ShadowFade, 0.0, 1.0);

	bool fadeCascades = csm.cascadeFading;
	float shadowAmount = 1.0;
	if (fadeCascades)
	{
		float cascadeTransitionFade = csm.cascadeTransitionFade;
		
		float c0 = smoothstep(csm.cascadeSplits[0] + cascadeTransitionFade * 0.5f, csm.cascadeSplits[0] - cascadeTransitionFade * 0.5f, ViewPosition.z);
		float c1 = smoothstep(csm.cascadeSplits[1] + cascadeTransitionFade * 0.5f, csm.cascadeSplits[1] - cascadeTransitionFade * 0.5f, ViewPosition.z);
		float c2 = smoothstep(csm.cascadeSplits[2] + cascadeTransitionFade * 0.5f, csm.cascadeSplits[2] - cascadeTransitionFade * 0.5f, ViewPosition.z);
		if (c0 > 0.0 && c0 < 1.0)
		{
			// Sample 0 & 1
			vec3 shadowMapCoords = (FragPosLightSpace[0].xyz / FragPosLightSpace[0].w);
			float shadowAmount0 = CalcShadow(shadowMap0, shadowMapCoords, dirLight.direction, N);
			shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = CalcShadow(shadowMap1, shadowMapCoords, dirLight.direction, N);

			shadowAmount = mix(shadowAmount0, shadowAmount1, c0);
		}
		else if (c1 > 0.0 && c1 < 1.0)
		{
			// Sample 1 & 2
			vec3 shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = CalcShadow(shadowMap1, shadowMapCoords, dirLight.direction, N);
			shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = CalcShadow(shadowMap2, shadowMapCoords, dirLight.direction, N);

			shadowAmount = mix(shadowAmount1, shadowAmount2, c1);
		}
		else if (c2 > 0.0 && c2 < 1.0)
		{
			// Sample 2 & 3
			vec3 shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = CalcShadow(shadowMap2, shadowMapCoords, dirLight.direction, N);
			shadowMapCoords = (FragPosLightSpace[3].xyz / FragPosLightSpace[3].w);
			float shadowAmount3 = CalcShadow(shadowMap3, shadowMapCoords, dirLight.direction, N);

			shadowAmount = mix(shadowAmount2, shadowAmount3, c2);
		}
		else
		{
			vec3 shadowMapCoords = (FragPosLightSpace[CascadeIndex].xyz / FragPosLightSpace[CascadeIndex].w);
			if(CascadeIndex == 0)
			{
				shadowAmount = CalcShadow(shadowMap0, shadowMapCoords, dirLight.direction, N);
			}
			if(CascadeIndex == 1)
			{
				shadowAmount = CalcShadow(shadowMap1, shadowMapCoords, dirLight.direction, N);
			}
			if(CascadeIndex == 2)
			{
				shadowAmount = CalcShadow(shadowMap2, shadowMapCoords, dirLight.direction, N);
			}
			if(CascadeIndex == 3)
			{
				shadowAmount = CalcShadow(shadowMap3, shadowMapCoords, dirLight.direction, N);
			}
			//shadowAmount = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture[CascadeIndex], shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture[CascadeIndex], shadowMapCoords);
		}
	}
	else
	{
		vec3 shadowMapCoords = (FragPosLightSpace[CascadeIndex].xyz / FragPosLightSpace[CascadeIndex].w);
			if(CascadeIndex == 0)
			{
				shadowAmount = CalcShadow(shadowMap0, shadowMapCoords, dirLight.direction, N);
			}
			if(CascadeIndex == 1)
			{
				shadowAmount = CalcShadow(shadowMap1, shadowMapCoords, dirLight.direction, N);
			}
			if(CascadeIndex == 2)
			{
				shadowAmount = CalcShadow(shadowMap2, shadowMapCoords, dirLight.direction, N);
			}
			if(CascadeIndex == 3)
			{
				shadowAmount = CalcShadow(shadowMap3, shadowMapCoords, dirLight.direction, N);
			}
		//shadowAmount = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture[CascadeIndex], shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture[CascadeIndex], shadowMapCoords);
	}

	float NdotL = dot(N, dirLight.direction);
	NdotL = smoothstep(0.0, 0.4, NdotL + 0.2);
	shadowAmount *= (NdotL * 1.0);

	return shadowAmount;
}

vec3 PbrDirectionalLighting(DirLight light, float roughness, float metallic, vec3 albedo, vec3 N, vec3 V, vec3 F0)
{
	vec3 directLighting = vec3(0.0); 

	// calculate per-light radiance
	vec3 L = normalize(light.direction);
	vec3 H = normalize(V + L);
       
    float attenuation = 1.0;
    vec3 radiance	  = light.diffuse * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
       
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
    vec3 specular	  = nominator / denominator;
    
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
	
	//float shadow = CalcShadow(FragPosLightSpace[0], N, L);       
	float shadow = CalcCascadedShadows(N);

    // add to outgoing radiance Lo
	directLighting = (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);// * intensity;

	return directLighting;
}


// PBR Functions from Learn Opengl (de Vries)
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
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// ----------------------------------------------------------------------------
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


