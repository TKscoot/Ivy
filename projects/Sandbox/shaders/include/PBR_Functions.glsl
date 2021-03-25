float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3  fresnelSchlick(float cosTheta, vec3 F0);
float gaSchlickG1(float cosTheta, float k);
float gaSchlickGGX(float cosLi, float cosLo, float roughness);
vec3  fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
const float PI = 3.141592;
const float Epsilon = 0.00001;

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

/*
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
*/
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

vec3 fresnelSchlickHazel(vec3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

const vec3 Fdielectric = vec3(0.04);
// Hazel Lighting Function
vec3 Lighting(DirLight light, vec3 F0, vec3 view, vec3 normal, float roughness, float metallic, float NdotV, vec3 albedo)
{
	vec3 result = vec3(0.0);
	vec3 Li = normalize(light.direction);
	vec3 Lradiance = vec3(1.0) * light.intensity;
	vec3 Lh = normalize(Li + view);

	// Calculate angles between surface normal and various light vectors.
	float cosLi = max(0.0, dot(normal, Li));
	float cosLh = max(0.0, dot(normal, Lh));

	vec3 F = fresnelSchlickHazel(F0, max(0.0, dot(Lh, view)));
	float D = ndfGGX(cosLh, roughness);
	float G = gaSchlickGGX(cosLi, NdotV, roughness);

	vec3 kd = (1.0 - F) * (1.0 - metallic);
	vec3 diffuseBRDF = kd * albedo;

	// Cook-Torrance
	vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * NdotV);

	result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;

	return result;
}

vec3 Lighting(PointLight light, vec3 albedo, vec3 F0, vec3 fragPos, vec3 view, vec3 normal, float roughness, float metallic)
{
    vec3 lightPosition = light.position;
    vec3 lightDiffuse  = light.diffuse;

    // calculate per-light radiance
    vec3 L = normalize(lightPosition - fragPos);
    vec3 H = normalize(view + L);

    float dist		  = length(lightPosition - fragPos);
    float attenuation = 1.0 / (dist * dist);
    vec3 radiance	  = lightDiffuse * attenuation * 15;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, view, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, view), 0.0), F0);

    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(normal, view), 0.0) * max(dot(normal, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
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
    float NdotL = max(dot(normal, L), 0.0);

    // add to outgoing radiance Lo
    return (kD * albedo / PI + specular) * radiance * NdotL * light.constant;
}

// ****************IBL****************
