#version 450 core
#extension GL_ARB_shading_language_include : require
#include "/LightStructs.glsl"
#include "/PBR_Functions.glsl"
//#include "/Shadow_Functions.glsl"

out vec4 FragColor;
out vec4 GodRayOcclusionColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;
in vec4 FragPosLightSpace[4];
in vec3 ViewPosition;
in mat4 Projection;
in mat4 View;

layout(binding = 0) uniform sampler2D   diffuseMap;
layout(binding = 1) uniform sampler2D   normalMap;
layout(binding = 2) uniform sampler2D   roughnessMap;
layout(binding = 3) uniform sampler2D   metallicMap;
layout(binding = 5) uniform sampler2D   aoMap;
layout(binding = 4) uniform sampler2D   brdfLutTexture;
layout(binding = 6) uniform samplerCube radianceMap;
layout(binding = 7) uniform samplerCube irradianceMap;
layout(binding = 8) uniform sampler2DArray u_ShadowMapTexture;
// ACHTUNG! NEXT SAMPLER BINDING == 12
//layout(binding = 12) uniform sampler2D  noiseTexture;


//uniform mat4 lightSpaceMatrices[16];
//uniform float cascadePlaneDistances[16];
//uniform int cascadeCount;   // number of frusta - 1
//uniform float farPlane;


//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool useNormalMap;
uniform bool useMetallicMap;
uniform bool useRoughnessMap;
uniform bool useIBL;

// Material
struct Material {
	vec2  tiling;
	vec3  ambient;
	vec3  diffuse;
	vec3  specular;
	float metallic;
	float roughness;
	float iblStrength;
};

struct CascadedShadowMapParams
{
	bool  oldShadows;
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


#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

uniform int pointLightSize; // TODO: check cpu side if this is bigger than MAX_POINT_LIGHTS
uniform int spotLightSize;

uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform Material material;

uniform CascadedShadowMapParams csm;

//const float PI = 3.14159265359;
//const float Epsilon = 0.00001;

uint  CascadeIndex = 0;
float ShadowFade   = 1.0;

// Function Prototypes
float CalcShadow(sampler2D shadowMap, vec3 fragPosLightSpace, vec3 lightPos, vec3 N);
float CalcCascadedShadows(vec3 N);
vec3 IBL(vec3 F0, vec3 Lr, vec3 normal, float roughness, float metallic, vec3 albedo, float NdotV, vec3 view);
vec3 RotateVectorAboutY(float angle, vec3 vec);

// Chernos Shadow method
float CalcShadowsCherno(vec3 normal, vec3 view);
float PCSS_DirectionalLight(sampler2D shadowMap, vec3 shadowCoords, float uvLightSize, vec3 normal);
float PCF_DirectionalLight(sampler2D shadowMap, vec3 shadowCoords, float uvRadius, vec3 normal);
float HardShadows_DirectionalLight(sampler2D shadowMap, vec3 shadowCoords, vec3 normal);
float FindBlockerDistance_DirectionalLight(sampler2D shadowMap, vec3 shadowCoords, float uvLightSize, vec3 normal);
float GetShadowBias(vec3 normal);
vec2 searchRegionRadiusUV(float zWorld);

float CalcShadowArr(sampler2DArray shadowMap, uint cascade, vec3 fragPosLightSpace, vec3 lightPos, vec3 N);


#include "/Shadow_Functions.glsl"

//MAIN
void main()
{
	vec2  uv		= TexCoords * material.tiling;
    vec3  albedo    = pow(texture(diffuseMap, uv).rgb, vec3(2.2));
	float alpha		= texture(diffuseMap, uv).a;
    float metallic  = material.metallic;
    float roughness = material.roughness;
	roughness = max(roughness, 0.05);

	if(alpha <= 0.1)
	{
		discard;
	}

	vec3 N = Normal;
	if(useNormalMap)
	{
		// obtain normal from normal map in range [0,1]
		N = texture(normalMap, uv).rgb;
		// transform normal vector to range [-1,1]
		N = normalize(N * 2.0 - 1.0);
		N = normalize(TBN * N);
	}

	if(useMetallicMap)
	{
		metallic = texture(metallicMap, uv).b;
	}

	if(useRoughnessMap)
	{
		roughness = texture(roughnessMap, uv).g;
	}

	vec3 V = normalize(viewPos - FragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)

	vec3 F0 = mix(Fdielectric, albedo, metallic);

	vec3 radiance = vec3(1.0);


	float NdotV = max(dot(N, V), 0.0);

	float shadow = 0.0;
	if(csm.oldShadows)
	{
		//shadow = 1 - CalcCascadedShadows(N);
	}
	else
	{
		shadow = CalcShadowsCherno(N, viewPos);
	}

	//shadow = CalcShadowChernoNew();




	uint cascadeIndex = 0;

	const uint SHADOW_MAP_CASCADE_COUNT = 4;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if (ViewPosition.z < csm.cascadeSplits[i])
			cascadeIndex = i + 1;
	}

	






	vec3 Lo = vec3(0.0);
	vec3 Lr = 2.0 * NdotV * N - V;

	vec3 iblContribution = vec3(0.0);
	if(useIBL)
	{
		iblContribution = IBL(F0, Lr, N, roughness, metallic, albedo, NdotV, V) * material.iblStrength;
	}
	
	vec3 lightContribution = Lighting(dirLight, F0, V, N, roughness, metallic, NdotV, albedo) * shadow;

	Lo = lightContribution + iblContribution;

	// Point Lights
	// Implementierung basierend auf topfs2's shadow-swan renderer (https://github.com/topfs2/shadow-swan/blob/master/shaders/pbr.fs)
	for(int i = 0; i < pointLightSize; ++i)
    {
		Lo += Lighting(pointLights[i], albedo, F0, FragPos, V, N, roughness, metallic);
	}

	vec3 ambient = vec3(0);

	vec3 color =  Lo;
	//color += ambient;

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

	color = pow(color, vec3(1.0/2.2));
	FragColor = vec4(color, alpha);
	GodRayOcclusionColor = vec4(0.0, 0.0, 0.0, 1.0);


	// FOG
	//float fogNear = 100.;
	//float fogFar = 120.;
	//vec4 fogColor = vec4(1.0, 0.4, 0.0, 1.0);
	//float fog = smoothstep(fogNear, fogFar, length(viewPos));
    //FragColor = vec4(mix(FragColor, fogColor, fog).rgb, alpha);


	//FragColor += vec4(ScreenSpaceReflection(1.0, N, viewPos, albedo, 0.5), 1.0);
}

float CalcShadow(sampler2D shadowMap, vec3 fragPosLightSpace, vec3 lightPos, vec3 N)
{

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace;// fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    //projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos);//normalize(lightPos - FragPos);
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

float CalcShadowArr(sampler2DArray shadowMap, uint cascade, vec3 fragPosLightSpace, vec3 lightPos, vec3 N)
{

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace;// fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    //projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, vec3(projCoords.xy * 0.5 + 0.5, cascade)).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos);//normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(vec2(projCoords.xy + vec2(x, y) * texelSize), cascade)).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

// Leicht abge�nderte Implementation basierend auf Yan Chernikovs Hazel Engine (https://github.com/TheCherno/Hazel)
/*
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
			float shadowAmount0 = CalcShadow(shadowMap0[0], shadowMapCoords, dirLight.direction, N);
			shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = CalcShadow(shadowMap0[1], shadowMapCoords, dirLight.direction, N);

			shadowAmount = mix(shadowAmount0, shadowAmount1, c0);
		}
		else if (c1 > 0.0 && c1 < 1.0)
		{
			// Sample 1 & 2
			vec3 shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = CalcShadow(shadowMap0[1], shadowMapCoords, dirLight.direction, N);
			shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = CalcShadow(shadowMap0[2], shadowMapCoords, dirLight.direction, N);

			shadowAmount = mix(shadowAmount1, shadowAmount2, c1);
		}
		else if (c2 > 0.0 && c2 < 1.0)
		{
			// Sample 2 & 3
			vec3 shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = CalcShadow(shadowMap0[2], shadowMapCoords, dirLight.direction, N);
			shadowMapCoords = (FragPosLightSpace[3].xyz / FragPosLightSpace[3].w);
			float shadowAmount3 = CalcShadow(shadowMap0[3], shadowMapCoords, dirLight.direction, N);

			shadowAmount = mix(shadowAmount2, shadowAmount3, c2);
		}
		else
		{
			vec3 shadowMapCoords = (FragPosLightSpace[CascadeIndex].xyz / FragPosLightSpace[CascadeIndex].w);
			shadowAmount = CalcShadow(shadowMap0[CascadeIndex], shadowMapCoords, dirLight.direction, N);
		}
	}
	else
	{
		vec3 shadowMapCoords = (FragPosLightSpace[CascadeIndex].xyz / FragPosLightSpace[CascadeIndex].w);
		shadowAmount = CalcShadow(shadowMap0[CascadeIndex], shadowMapCoords, dirLight.direction, N);
	}

	float NdotL = dot(N, dirLight.direction);
	NdotL = smoothstep(0.0, 0.4, NdotL + 0.2);
	shadowAmount *= (NdotL * 1.0);

	return shadowAmount;
}
*/

vec3 RotateVectorAboutY(float angle, vec3 vec)
{
    angle = radians(angle);
    mat3x3 rotationMatrix ={vec3(cos(angle),0.0,sin(angle)),
                            vec3(0.0,1.0,0.0),
                            vec3(-sin(angle),0.0,cos(angle))};
    return rotationMatrix * vec;
}
vec3 IBL(vec3 F0, vec3 Lr, vec3 normal, float roughness, float metallic, vec3 albedo, float NdotV, vec3 view)
{
	vec3 irradiance = texture(irradianceMap, normal).rgb;
	vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
	vec3 kd = (1.0 - F) * (1.0 - metallic);
	vec3 diffuseIBL = albedo * irradiance;

	int u_EnvRadianceTexLevels = textureQueryLevels(radianceMap);
	float NoV = clamp(NdotV, 0.0, 1.0);
	vec3 R = 2.0 * dot(view, normal) * normal - view;
	vec3 specularIrradiance = textureLod(radianceMap, RotateVectorAboutY(/*u_EnvMapRotation*/0.0, Lr), (roughness) * u_EnvRadianceTexLevels).rgb;

	// Sample BRDF Lut, 1.0 - roughness for y-coord because texture was generated (in Sparky) for gloss model
	vec2 specularBRDF = texture(brdfLutTexture, vec2(NdotV, 1.0 - roughness)).rg;
	vec3 specularIBL = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);

	return kd * diffuseIBL + specularIBL;
}








//float u_light_zNear = 0.0; // 0.01 gives artifacts? maybe because of ortho proj?
//float u_light_zFar = 10000.0;
//vec2 u_lightRadiusUV = vec2(0.05);

vec2 searchRegionRadiusUV(float zWorld)
{
    return u_lightRadiusUV * (zWorld - u_light_zNear) / zWorld;
}

float GetShadowBias(vec3 normal)
{
	const float MINIMUM_SHADOW_BIAS = 0.002;
	float bias = max(MINIMUM_SHADOW_BIAS * (1.0 - dot(normal, dirLight.direction)), MINIMUM_SHADOW_BIAS);
	return bias;
}

float FindBlockerDistance_DirectionalLight(sampler2D shadowMap, vec3 shadowCoords, float uvLightSize, vec3 normal)
{
	float bias = GetShadowBias(normal);

	int numBlockerSearchSamples = 32;
	int blockers = 0;
	float avgBlockerDistance = 0;

	float zEye = -(csm.lightView * vec4(FragPos, 1.0)).z;
	vec2 searchWidth = searchRegionRadiusUV(zEye);
	for (int i = 0; i < numBlockerSearchSamples; i++)
	{
		float z = texture(shadowMap, shadowCoords.xy + SamplePoisson(i) * searchWidth).r;
		if (z < (shadowCoords.z - bias))
		{
			blockers++;
			avgBlockerDistance += z;
		}
	}

	if (blockers > 0)
		return avgBlockerDistance / float(blockers);

	return -1;
}

float HardShadows_DirectionalLight(sampler2D shadowMap, vec3 shadowCoords, vec3 normal)
{
	float bias = GetShadowBias(normal);
	float z = texture(shadowMap, shadowCoords.xy).x;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, shadowCoords.xy + vec2(x, y) * texelSize).r;
            shadow += shadowCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

	return 1.0 - shadow/*1.0 - step(z + bias, shadowCoords.z)  * ShadowFade */;

}

float PCF_DirectionalLight(sampler2D shadowMap, vec3 shadowCoords, float uvRadius, vec3 normal)
{
	float bias = GetShadowBias(normal);
	int numPCFSamples = 32;
	float sum = 0;
	for (int i = 0; i < numPCFSamples; i++)
	{
		float z = texture(shadowMap, shadowCoords.xy + SamplePoisson(i)  * uvRadius).r;
		sum += (z < (shadowCoords.z - bias)) ? 1 : 0;
	}
	return sum / numPCFSamples;
}

float PCSS_DirectionalLight(sampler2D shadowMap, vec3 shadowCoords, float uvLightSize, vec3 normal)
{
	float blockerDistance = FindBlockerDistance_DirectionalLight(shadowMap, shadowCoords, uvLightSize, normal);
	if (blockerDistance == -1)
		return 1;

	float penumbraWidth = (shadowCoords.z - blockerDistance) / blockerDistance;

	float NEAR = 0.01; // Should this value be tweakable?
	float uvRadius = penumbraWidth * uvLightSize * NEAR / shadowCoords.z;
	return 1.0 - PCF_DirectionalLight(shadowMap, shadowCoords, uvRadius, normal) * ShadowFade;
}

/*
float CalcShadowsCherno(vec3 normal)
{
const uint SHADOW_MAP_CASCADE_COUNT = 4;
	for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if(ViewPosition.z < csm.cascadeSplits[i])
			CascadeIndex = i + 1;
	}

	float shadowDistance = csm.maxShadowDistance;
	float transitionDistance = csm.shadowFade;
	float dist = length(ViewPosition);
	ShadowFade = dist - (shadowDistance - transitionDistance);
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
			float shadowAmount0 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[0], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[0], shadowMapCoords, normal);
			shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[1], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[1], shadowMapCoords, normal);

			shadowAmount = mix(shadowAmount0, shadowAmount1, c0);
		}
		else if (c1 > 0.0 && c1 < 1.0)
		{
			// Sample 1 & 2
			vec3 shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[1], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[1], shadowMapCoords, normal);
			shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[2], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[2], shadowMapCoords, normal);

			shadowAmount = mix(shadowAmount1, shadowAmount2, c1);
		}
		else if (c2 > 0.0 && c2 < 1.0)
		{
			// Sample 2 & 3
			vec3 shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[2], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[2], shadowMapCoords, normal);
			shadowMapCoords = (FragPosLightSpace[3].xyz / FragPosLightSpace[3].w);
			float shadowAmount3 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[3], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[3], shadowMapCoords, normal);

			shadowAmount = mix(shadowAmount2, shadowAmount3, c2);
		}
		else
		{
			vec3 shadowMapCoords = (FragPosLightSpace[CascadeIndex].xyz / FragPosLightSpace[CascadeIndex].w);
			shadowAmount = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[CascadeIndex], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[CascadeIndex], shadowMapCoords, normal);
		}
	}
	else
	{
		vec3 shadowMapCoords = (FragPosLightSpace[CascadeIndex].xyz / FragPosLightSpace[CascadeIndex].w);
		shadowAmount = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[CascadeIndex], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[CascadeIndex], shadowMapCoords, normal);
	}

	float NdotL = dot(normal, dirLight.direction);
	NdotL = smoothstep(0.0, 0.4, NdotL + 0.2);
	shadowAmount *= (NdotL * 1.0);

	return shadowAmount;
}
*/

float CalcShadowsCherno(vec3 normal, vec3 view)
{
	uint cascadeIndex = 0;

	const uint SHADOW_MAP_CASCADE_COUNT = 4;
	for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if(ViewPosition.z < csm.cascadeSplits[i])
			CascadeIndex = i + 1;
	}

	float shadowDistance = csm.maxShadowDistance;
	float transitionDistance = csm.shadowFade;
	float dist = length(view);
	ShadowFade = dist - (shadowDistance - transitionDistance);
	ShadowFade /= transitionDistance;
	ShadowFade = clamp(1.0 - ShadowFade, 0.0, 1.0);

	bool fadeCascades = csm.cascadeFading;
	float shadowAmount = 1.0;
	if (fadeCascades)
	{
		float cascadeTransitionFade = csm.cascadeTransitionFade;

		float c0 = smoothstep(csm.cascadeSplits[0] + cascadeTransitionFade * 0.5f, csm.cascadeSplits[0] - cascadeTransitionFade * 0.5f, viewPos.z);
		float c1 = smoothstep(csm.cascadeSplits[1] + cascadeTransitionFade * 0.5f, csm.cascadeSplits[1] - cascadeTransitionFade * 0.5f, viewPos.z);
		float c2 = smoothstep(csm.cascadeSplits[2] + cascadeTransitionFade * 0.5f, csm.cascadeSplits[2] - cascadeTransitionFade * 0.5f, viewPos.z);
		if (c0 > 0.0 && c0 < 1.0)
		{
			// Sample 0 & 1
			vec3 shadowMapCoords = (FragPosLightSpace[0].xyz / FragPosLightSpace[0].w);
			float shadowAmount0 = csm.softShadows ? PCSS_DirectionalLightArr(u_ShadowMapTexture, 0, shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLightArr(u_ShadowMapTexture, 0, shadowMapCoords, normal);
			shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = csm.softShadows ? PCSS_DirectionalLightArr(u_ShadowMapTexture, 1, shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLightArr(u_ShadowMapTexture, 1, shadowMapCoords, normal);

			shadowAmount = mix(shadowAmount0, shadowAmount1, c0);
		}
		else if (c1 > 0.0 && c1 < 1.0)
		{
			// Sample 1 & 2
			vec3 shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = csm.softShadows ? PCSS_DirectionalLightArr(u_ShadowMapTexture, 1, shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLightArr(u_ShadowMapTexture, 1, shadowMapCoords, normal);
			shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = csm.softShadows ? PCSS_DirectionalLightArr(u_ShadowMapTexture, 2, shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLightArr(u_ShadowMapTexture, 2, shadowMapCoords, normal);

			shadowAmount = mix(shadowAmount1, shadowAmount2, c1);
		}
		else if (c2 > 0.0 && c2 < 1.0)
		{
			// Sample 2 & 3
			vec3 shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = csm.softShadows ? PCSS_DirectionalLightArr(u_ShadowMapTexture, 2, shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLightArr(u_ShadowMapTexture, 2, shadowMapCoords, normal);
			shadowMapCoords = (FragPosLightSpace[3].xyz / FragPosLightSpace[3].w);
			float shadowAmount3 = csm.softShadows ? PCSS_DirectionalLightArr(u_ShadowMapTexture, 3, shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLightArr(u_ShadowMapTexture, 3, shadowMapCoords, normal);

			shadowAmount = mix(shadowAmount2, shadowAmount3, c2);
		}
		else
		{
			vec3 shadowMapCoords = (FragPosLightSpace[cascadeIndex].xyz / FragPosLightSpace[cascadeIndex].w);
			shadowAmount = csm.softShadows ? PCSS_DirectionalLightArr(u_ShadowMapTexture, cascadeIndex, shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLightArr(u_ShadowMapTexture, cascadeIndex, shadowMapCoords, normal);
		}
	}
	else
	{
		vec3 shadowMapCoords = (FragPosLightSpace[cascadeIndex].xyz / FragPosLightSpace[cascadeIndex].w);
		shadowAmount = csm.softShadows ? PCSS_DirectionalLightArr(u_ShadowMapTexture, cascadeIndex, shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLightArr(u_ShadowMapTexture, cascadeIndex, shadowMapCoords, normal);
	}


	//float NdotL = dot(normal, dirLight.direction);
	//NdotL = smoothstep(0.0, 0.4, NdotL + 0.2);
	//shadowAmount *= (NdotL * 1.0);
	return shadowAmount;

}
/*
float CalcShadowsCherno1(vec3 normal)
{
const uint SHADOW_MAP_CASCADE_COUNT = 4;
	for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if(ViewPosition.z < csm.cascadeSplits[i])
			CascadeIndex = i + 1;
	}

	float shadowDistance = csm.maxShadowDistance;
	float transitionDistance = csm.shadowFade;
	float dist = length(ViewPosition);
	ShadowFade = dist - (shadowDistance - transitionDistance);
	ShadowFade /= transitionDistance;
	ShadowFade = clamp(1.0 - ShadowFade, 0.0, 1.0);

	bool fadeCascades = csm.cascadeFading;
	float shadowAmount = 1.0;
	if (fadeCascades)
	{
		float cascadeTransitionFade = csm.cascadeTransitionFade;

		float c0 = smoothstep(csm.cascadeSplits[0] + cascadeTransitionFade * 0.5f, csm.cascadeSplits[0] - cascadeTransitionFade * 0.5f, viewPos.z);
		float c1 = smoothstep(csm.cascadeSplits[1] + cascadeTransitionFade * 0.5f, csm.cascadeSplits[1] - cascadeTransitionFade * 0.5f, viewPos.z);
		float c2 = smoothstep(csm.cascadeSplits[2] + cascadeTransitionFade * 0.5f, csm.cascadeSplits[2] - cascadeTransitionFade * 0.5f, viewPos.z);
		if (c0 > 0.0 && c0 < 1.0)
		{
			// Sample 0 & 1
			vec3 shadowMapCoords = (FragPosLightSpace[0].xyz / FragPosLightSpace[0].w);
			float shadowAmount0 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[0], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[0], shadowMapCoords, normal);
			shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[1], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[1], shadowMapCoords, normal);

			shadowAmount = mix(shadowAmount0, shadowAmount1, c0);
		}
		else if (c1 > 0.0 && c1 < 1.0)
		{
			// Sample 1 & 2
			vec3 shadowMapCoords = (FragPosLightSpace[1].xyz / FragPosLightSpace[1].w);
			float shadowAmount1 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[1], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[1], shadowMapCoords, normal);
			shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[2], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[2], shadowMapCoords, normal);

			shadowAmount = mix(shadowAmount1, shadowAmount2, c1);
		}
		else if (c2 > 0.0 && c2 < 1.0)
		{
			// Sample 2 & 3
			vec3 shadowMapCoords = (FragPosLightSpace[2].xyz / FragPosLightSpace[2].w);
			float shadowAmount2 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[2], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[2], shadowMapCoords, normal);
			shadowMapCoords = (FragPosLightSpace[3].xyz / FragPosLightSpace[3].w);
			float shadowAmount3 = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[3], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[3], shadowMapCoords, normal);

			shadowAmount = mix(shadowAmount2, shadowAmount3, c2);
		}
		else
		{
			vec3 shadowMapCoords = (FragPosLightSpace[CascadeIndex].xyz / FragPosLightSpace[CascadeIndex].w);
			shadowAmount = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[CascadeIndex], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[CascadeIndex], shadowMapCoords, normal);
		}
	}
	else
	{
		vec3 shadowMapCoords = (FragPosLightSpace[CascadeIndex].xyz / FragPosLightSpace[CascadeIndex].w);
		shadowAmount = csm.softShadows ? PCSS_DirectionalLight(shadowMap0[CascadeIndex], shadowMapCoords, csm.lightSize, normal) : HardShadows_DirectionalLight(shadowMap0[CascadeIndex], shadowMapCoords, normal);
	}

	float NdotL = dot(normal, dirLight.direction);
	NdotL = smoothstep(0.0, 0.4, NdotL + 0.2);
	shadowAmount *= (NdotL * 1.0);

	return shadowAmount;
}
*/