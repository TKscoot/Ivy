float u_light_zNear = 0.0; // 0.01 gives artifacts? maybe because of ortho proj?
float u_light_zFar = 10000.0;
vec2 u_lightRadiusUV = vec2(0.05);

// Penumbra


float HardShadows_DirectionalLightArr(sampler2DArray shadowMap, uint cascade, vec3 shadowCoords, vec3 normal)
{
	float bias = GetShadowBias(normal);
	float shadowMapDepth = texture(shadowMap, vec3(shadowCoords.xy * 0.5 + 0.5, cascade)).x;
	return step(shadowCoords.z, shadowMapDepth + bias) * ShadowFade;

}

float SearchRegionRadiusUV(float zWorld)
{
	const float light_zNear = 0.0; // 0.01 gives artifacts? maybe because of ortho proj?
	const float lightRadiusUV = 0.05;
	return lightRadiusUV * (zWorld - light_zNear) / zWorld;

}


const vec2 PoissonDistribution[64] = vec2[](
	vec2(-0.884081, 0.124488),
	vec2(-0.714377, 0.027940),
	vec2(-0.747945, 0.227922),
	vec2(-0.939609, 0.243634),
	vec2(-0.985465, 0.045534),
	vec2(-0.861367, -0.136222),
	vec2(-0.881934, 0.396908),
	vec2(-0.466938, 0.014526),
	vec2(-0.558207, 0.212662),
	vec2(-0.578447, -0.095822),
	vec2(-0.740266, -0.095631),
	vec2(-0.751681, 0.472604),
	vec2(-0.553147, -0.243177),
	vec2(-0.674762, -0.330730),
	vec2(-0.402765, -0.122087),
	vec2(-0.319776, -0.312166),
	vec2(-0.413923, -0.439757),
	vec2(-0.979153, -0.201245),
	vec2(-0.865579, -0.288695),
	vec2(-0.243704, -0.186378),
	vec2(-0.294920, -0.055748),
	vec2(-0.604452, -0.544251),
	vec2(-0.418056, -0.587679),
	vec2(-0.549156, -0.415877),
	vec2(-0.238080, -0.611761),
	vec2(-0.267004, -0.459702),
	vec2(-0.100006, -0.229116),
	vec2(-0.101928, -0.380382),
	vec2(-0.681467, -0.700773),
	vec2(-0.763488, -0.543386),
	vec2(-0.549030, -0.750749),
	vec2(-0.809045, -0.408738),
	vec2(-0.388134, -0.773448),
	vec2(-0.429392, -0.894892),
	vec2(-0.131597, 0.065058),
	vec2(-0.275002, 0.102922),
	vec2(-0.106117, -0.068327),
	vec2(-0.294586, -0.891515),
	vec2(-0.629418, 0.379387),
	vec2(-0.407257, 0.339748),
	vec2(0.071650, -0.384284),
	vec2(0.022018, -0.263793),
	vec2(0.003879, -0.136073),
	vec2(-0.137533, -0.767844),
	vec2(-0.050874, -0.906068),
	vec2(0.114133, -0.070053),
	vec2(0.163314, -0.217231),
	vec2(-0.100262, -0.587992),
	vec2(-0.004942, 0.125368),
	vec2(0.035302, -0.619310),
	vec2(0.195646, -0.459022),
	vec2(0.303969, -0.346362),
	vec2(-0.678118, 0.685099),
	vec2(-0.628418, 0.507978),
	vec2(-0.508473, 0.458753),
	vec2(0.032134, -0.782030),
	vec2(0.122595, 0.280353),
	vec2(-0.043643, 0.312119),
	vec2(0.132993, 0.085170),
	vec2(-0.192106, 0.285848),
	vec2(0.183621, -0.713242),
	vec2(0.265220, -0.596716),
	vec2(-0.009628, -0.483058),
	vec2(-0.018516, 0.435703)
	);

const vec2 poissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
	);

vec2 SamplePoisson(int index)
{
	return PoissonDistribution[index % 64];
}


float FindBlockerDistance_DirectionalLightArr(sampler2DArray shadowMap, uint cascade, vec3 shadowCoords, float uvLightSize, vec3 normal)
{
	float bias = GetShadowBias(normal);

	int numBlockerSearchSamples = 64;
	int blockers = 0;
	float avgBlockerDistance = 0;

	float searchWidth = SearchRegionRadiusUV(shadowCoords.z);
	for (int i = 0; i < numBlockerSearchSamples; i++)
	{
		float z = texture(shadowMap, vec3((shadowCoords.xy * 0.5 + 0.5) + SamplePoisson(i) * searchWidth, cascade)).r;
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

float PCF_DirectionalLightArr(sampler2DArray shadowMap, uint cascade, vec3 shadowCoords, float uvRadius, vec3 normal)
{
	float bias = GetShadowBias(normal);
	int numPCFSamples = 64;

	float sum = 0;
	for (int i = 0; i < numPCFSamples; i++)
	{
		vec2 offset = SamplePoisson(i) * uvRadius;
		float z = texture(shadowMap, vec3((shadowCoords.xy * 0.5 + 0.5) + offset, cascade)).r;
		sum += step(shadowCoords.z - bias, z);
	}
	return sum / numPCFSamples;
}

float NV_PCF_DirectionalLightArr(sampler2DArray shadowMap, uint cascade, vec3 shadowCoords, float uvRadius, vec3 normal)
{
	float bias = GetShadowBias(normal);

	float sum = 0;
	for (int i = 0; i < 16; i++)
	{
		vec2 offset = poissonDisk[i] * uvRadius;
		float z = texture(shadowMap, vec3((shadowCoords.xy * 0.5 + 0.5) + offset, cascade)).r;
		sum += step(shadowCoords.z - bias, z);
	}
	return sum / 16.0f;
}

float PCSS_DirectionalLightArr(sampler2DArray shadowMap, uint cascade, vec3 shadowCoords, float uvLightSize, vec3 normal)
{
	float blockerDistance = FindBlockerDistance_DirectionalLightArr(shadowMap, cascade, shadowCoords, uvLightSize, normal);
	if (blockerDistance == -1) // No occlusion
		return 1.0f;

	float penumbraWidth = (shadowCoords.z - blockerDistance) / blockerDistance;

	float NEAR = 0.01; // Should this value be tweakable?
	float uvRadius = penumbraWidth * uvLightSize * NEAR / shadowCoords.z; // Do we need to divide by shadowCoords.z?
	uvRadius = min(uvRadius, 0.002f);
	return PCF_DirectionalLightArr(shadowMap, cascade, shadowCoords, uvRadius, normal) * ShadowFade;
}

/////////////////////////////////////////////