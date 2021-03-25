#version 450 core
#extension GL_ARB_shading_language_include : require
#include "/remap.glsl"

out vec4 FragColor;

in vec2 TexCoords;
in vec2 Pos;

layout(binding = 0) uniform sampler2D sceneColorMap;
layout(binding = 1) uniform sampler2D sceneDepthMap;
layout(binding = 2) uniform sampler2D godrayOcclusionMap;

uniform mat4 World;		
uniform mat4 View;						
uniform mat4 Projection;				
uniform mat4 WorldViewProjection;		
uniform mat4 PreviousView;
uniform mat4 PreviousProjection;
uniform mat4 PreviousWorldViewProjection;
uniform mat4 InverseViewProjection;	
uniform vec2 WindowResolution;	
uniform vec3 CameraPosition;
uniform vec3 PreviousCameraPosition;
uniform float NearPlane;
uniform float FarPlane;
uniform float FrameTime;
uniform int  Tonemap;	
uniform bool UseDepthOfField;
uniform bool UseMotionBlur;
uniform float MotionBlurIntensity;
uniform float DofThreshold;

struct GodrayParameters
{
	float density;
	float weight;
	float decay;
	float exposure;
	int numSamples;
};

GodrayParameters godrayParams;


// Tonemapping implementation from shadow-swan by Tobias Arrskog (https://github.com/topfs2/shadow-swan/blob/master/shaders/tonemap.glsl)
vec3 simpleReinhardToneMapping(vec3 color);
vec3 lumaBasedReinhardToneMapping(vec3 color);
vec3 RomBinDaHouseToneMapping(vec3 color);
vec3 filmicToneMapping(vec3 color);
vec3 Uncharted2ToneMapping(vec3 color);
vec3 tonemapAuto(vec3 color);

// Depth of field and motion blur
void DoF(inout vec3 color);
float depthStrength();
void SEUSmotionBlur(inout vec3 color, float depth);
vec3 GaussianBlur();

float LinearizeDepth(float d,float zNear,float zFar)
{
    float z_n = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}


//Godrays
vec3 godrays(
	float density,
	float weight,
	float decay,
	float exposure,
	int numSamples,
	sampler2D occlusionTexture,
	vec2 screenSpaceLightPos,
	vec2 uv);

// Helper functions
float rand(vec2 co);

float focusDistance =  10.0f;
float farDof = 5.0f;
float nearDof = 0.0001f;
float falloff = 2.0f;
float minStrength = 0.0f;

float draw_circle(vec2 coord, float radius) {
    return step(length(coord), radius);
}

// =====================MAIN=====================
void main()
{
	const float gamma     = 2.2;
	const float pureWhite = 1.0;
	
	vec3 color = texture2D(sceneColorMap, TexCoords).rgb;
	float depth = texture2D(sceneDepthMap, TexCoords).r;
	//depth = LinearizeDepth(depth, NearPlane, FarPlane);

	if(UseMotionBlur)
	{
		SEUSmotionBlur(color, depth);
	}
	if(UseDepthOfField)
	{
		float dofStrength = depthStrength();
		vec3 blurred = GaussianBlur();

		color = mix(blurred, color, dofStrength);
	}

	vec4 sunPos = Projection * View *  vec4(-2.0, 4.0, -1.0, 1.0);
	vec2 screenSpaceSunPos = vec2(0.0);
	screenSpaceSunPos.x = sunPos.x / sunPos.w;
	screenSpaceSunPos.y = sunPos.y / sunPos.w;

	//color += godrays(
	//godrayParams.density,	 
	//godrayParams.weight, 
	//godrayParams.decay,	 
	//godrayParams.exposure , 
	//godrayParams.numSamples,
	//godrayOcclusionMap,
	//screenSpaceSunPos,
	//TexCoords
	//);

	//color += godrays(
	//1.0,	 
	//0.01, 
	//1.0,	 
	//1.0, 
	//128,
	//godrayOcclusionMap,
	//screenSpaceSunPos,
	//TexCoords
	//);

	const vec4 backgroundColor = vec4(0.18,0.18,0.18,1);
	const vec4 crosshairsColor = vec4(1,0,0,1);
	const vec2 centerUV = vec2(0.5, 0.5);
	const int thickness = 1;
	const int len = 8;


    ivec2 fragCoordi = ivec2(gl_FragCoord.xy);
    ivec2 center = ivec2(WindowResolution.xy * centerUV);
    ivec2 d = abs(center - fragCoordi);

	
	if (min(d.x, d.y) < thickness && max(d.x,d.y) < len)
    {
        color = vec3(1,0.18,0.18);
    }

	// gamma correct
	color = pow(color, vec3(1.0/2.2)); 

	// tonemapping
	vec3 mappedColor = tonemapAuto(color) ;

	// Final color output!
	FragColor = vec4(mappedColor, texture2D(sceneColorMap, TexCoords).a);
}

vec3 simpleReinhardToneMapping(vec3 color)
{
	float exposure = 1.5;
	color *= exposure/(1. + color / exposure);
	return color;
}

vec3 lumaBasedReinhardToneMapping(vec3 color)
{
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma / (1. + luma);
	color *= toneMappedLuma / luma;
	return color;
}

vec3 RomBinDaHouseToneMapping(vec3 color)
{
	color = exp( -1.0 / ( 2.72*color + 0.15 ) );
	return color;
}

vec3 filmicToneMapping(vec3 color)
{
	color = max(vec3(0.), color - vec3(0.004));
	color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
	return color;
}

vec3 Uncharted2ToneMapping(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float exposure = 2.;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	return color;
}

vec3 tonemapAuto(vec3 color)
{
	switch (Tonemap) {
	case 0:
		break;
	case 1:
		color = simpleReinhardToneMapping(color);
		break;
	case 2:
		color = lumaBasedReinhardToneMapping(color);
		break;
	case 3:
		color = RomBinDaHouseToneMapping(color);
		break;
	case 4:
		color = Uncharted2ToneMapping(color);
		break;
	case 5:
		color = filmicToneMapping(color);
		break;
	}
	return color;
}

// Depth of Field and Motion Blur

void DoF(inout vec3 color)
{
	float depth = texture2D(sceneDepthMap, TexCoords).r;
	depth = LinearizeDepth(depth, NearPlane, FarPlane);
 
	float sampleOffset = depth/500.0;
 
	//vec4 col = vec4(0.0);
	color += texture2D(sceneColorMap, TexCoords.st + vec2(-sampleOffset,-sampleOffset)).rgb * 1.0;
	color += texture2D(sceneColorMap, TexCoords.st + vec2( 0.0         ,-sampleOffset)).rgb * 2.0;
	color += texture2D(sceneColorMap, TexCoords.st + vec2( sampleOffset,-sampleOffset)).rgb * 1.0;
				 
	color += texture2D(sceneColorMap, TexCoords.st + vec2(-sampleOffset, 0.0     )).rgb * 2.0;
	color += texture2D(sceneColorMap, TexCoords.st + vec2( 0.0         , 0.0     )).rgb * 4.0;
	color += texture2D(sceneColorMap, TexCoords.st + vec2( sampleOffset, 0.0     )).rgb * 2.0;
				 
	color += texture2D(sceneColorMap, TexCoords.st + vec2( sampleOffset, sampleOffset)).rgb * 1.0;
	color += texture2D(sceneColorMap, TexCoords.st + vec2( 0.0         , sampleOffset)).rgb * 2.0;
	color += texture2D(sceneColorMap, TexCoords.st + vec2(-sampleOffset, sampleOffset)).rgb * 1.0;
 
	color /= 16.0;
}

// godrays
vec3 godrays(
	float density,
	float weight,
	float decay,
	float exposure,
	int numSamples,
	sampler2D occlusionTexture,
	vec2 screenSpaceLightPos,
	vec2 uv
	) 
	{

	vec3 fragColor = vec3(0.0,0.0,0.0);

	vec2 deltaTextCoord = vec2( uv - screenSpaceLightPos.xy );

	vec2 textCoo = uv.xy ;
	deltaTextCoord *= (1.0 /  float(numSamples)) * density;
	float illuminationDecay = 1.0;


	for(int i=0; i < numSamples ; i++){

		textCoo -= deltaTextCoord;
		vec3 samp = texture2D(occlusionTexture, textCoo   ).xyz;
		samp *= illuminationDecay * weight;
		fragColor += samp;
		illuminationDecay *= decay;
	}

	fragColor *= exposure;

	return fragColor;


}

// Helper functions
float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


void SEUSmotionBlur(inout vec3 color, float depth) 
{
	vec4 currentPosition = vec4(TexCoords.x * 2.0f - 1.0f, TexCoords.y * 2.0f - 1.0f, 2.0f * depth - 1.0f, 1.0f);

	vec4 fragposition = inverse(Projection) * currentPosition;
	fragposition = inverse(View) * fragposition;
	fragposition /= fragposition.w;
	fragposition.xyz += CameraPosition;

	vec4 previousPosition = fragposition;
	previousPosition.xyz -= PreviousCameraPosition;
	previousPosition = PreviousView * previousPosition;
	previousPosition = PreviousProjection * previousPosition;
	previousPosition /= previousPosition.w;

	vec2 velocity = (currentPosition - previousPosition).st * 0.1f * (1.0 / FrameTime) * 0.012 * MotionBlurIntensity;
	float maxVelocity = 0.05f;
		 velocity = clamp(velocity, vec2(-maxVelocity), vec2(maxVelocity));

	int samples = 0;

	float dither = rand(TexCoords.st).x * 1.0;

	color.rgb = vec3(0.0f);

	for (int i = -2; i <= 2; ++i) {
		vec2 coord = TexCoords.st + velocity * (float(i + dither) / 2.0);
			 //coord += vec2(dither) * 1.0f * velocity;

		if (coord.x > 0.0f && coord.x < 1.0f && coord.y > 0.0f && coord.y < 1.0f) {

			color += texture(sceneColorMap, coord).rgb;
			samples += 1;

		}
	}

	color.rgb /= samples;


}


// Working Gaussian Blur (NOT MOTION BLUR!) From https://www.shadertoy.com/view/Xltfzj
vec3 GaussianBlur()
{
	float Pi = 6.28318530718; // Pi*2
	
	// GAUSSIAN BLUR SETTINGS {{{
	float Directions =32.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
	float Quality = 6.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
	float Size = 16.0; // BLUR SIZE (Radius)
	// GAUSSIAN BLUR SETTINGS }}}
	
	vec2 Radius = Size/textureSize(sceneColorMap, 0);
	
	vec3 result = texture(sceneColorMap, TexCoords).rgb;
	vec2 uv = TexCoords;
	// Blur calculations
	for( float d=0.0; d<Pi; d+=Pi/Directions)
	{
	for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)
	{
		result += texture( sceneColorMap, uv+vec2(cos(d),sin(d))*Radius*i).rgb;
	}
	}
	
	result /= Quality * Directions - 15.0;
	
	return result;

}

float depthStrength()
{
	float centerDepth = texture2D(sceneDepthMap, vec2(.5, .5)).r;
	float dofStrength = 1;

	if(centerDepth > DofThreshold)
	{
		dofStrength = 1.0;
		return mix(minStrength, 1.0f,dofStrength);
	}

	float dist = texture2D(sceneDepthMap, TexCoords).r;

	float z = (2 * NearPlane) / (FarPlane + NearPlane - dist * (FarPlane - NearPlane));
	float centerZ = (2 * NearPlane) / (FarPlane + NearPlane - centerDepth * (FarPlane - NearPlane));
	
	float sFarDof = (focusDistance+farDof)/FarPlane;
	float sNearDof = (focusDistance-nearDof)/FarPlane;    
	 
	if(z > sFarDof){
	   //dofStrength = ((sFarDof)/(z));
		dofStrength = ((sFarDof)/(z));
	}else if (z < sNearDof) {
	   dofStrength = ((z)/(sNearDof));
	}    
	

	dofStrength = clamp(1-dofStrength,0.0f,1.0f);
	dofStrength *= falloff;
	dofStrength =clamp(1-dofStrength,0.0f,1.0f);




	return mix(minStrength, 1.0f,dofStrength);
}