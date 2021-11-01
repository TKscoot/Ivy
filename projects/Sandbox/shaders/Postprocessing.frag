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
uniform float ElapsedTime;
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

vec3 DepthOfField(vec3 color, vec2 texCoord, float focusPoint, float focusScale, vec2 texelSize);


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

vec3 snowing(in vec2 uv);

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
		/*
		float dofStrength = depthStrength();
		vec3 blurred = GaussianBlur();

		color = mix(blurred, color, dofStrength);
		*/
		ivec2 texSize = textureSize(sceneColorMap, 0);
		vec2 fTexSize = vec2(float(texSize.x), float(texSize.y));
		float centerDepth = LinearizeDepth(texture(sceneDepthMap, vec2(0.5f)).r, 0.1, FarPlane);// *uFar;
		//float centerDepth = texture(sceneDepthMap, vec2(0.5f)).r;
		float focusPoint = centerDepth;
		float focusScale = 1.5f;

		color = DepthOfField(color, TexCoords, focusPoint, focusScale, 1.0f / fTexSize);
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

	// SNOW    
	//vec2 uv = WindowResolution.xy + vec2(1.,WindowResolution.y/WindowResolution.x)*gl_FragCoord.xy / WindowResolution.xy;
	//
	//vec3 snow = snowing(uv.xy);
	//color += snow;

	// gamma correct
	//color = pow(color, vec3(1.0/2.2)); 

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


#define LAYERS 66

#define DEPTH .3
#define WIDTH .4
#define SPEED .6

#define DEPTH2 .1
#define WIDTH2 .3
#define SPEED2 .1

vec3 snowing(in vec2 uv)
{
	float iTime = ElapsedTime;

  	const mat3 p = mat3(13.323122,23.5112,21.71123,21.1212,28.7312,11.9312,21.8112,14.7212,61.3934);
	vec3 acc = vec3(0.0);
	float dof = 5.*sin(iTime*.1);
	for (int i=0;i<LAYERS;i++) {
		float fi = float(i);
		vec2 q = uv*(1.+fi*DEPTH);
		q += vec2(q.y*(WIDTH*mod(fi*7.238917,1.)-WIDTH*.5),SPEED*iTime/(1.+fi*DEPTH*.03));
		vec3 n = vec3(floor(q),31.189+fi);
		vec3 m = floor(n)*.00001 + fract(n);
		vec3 mp = (31415.9+m)/fract(p*m);
		vec3 r = fract(mp);
		vec2 s = abs(mod(q,1.)-.5+.9*r.xy-.45);
		s += .01*abs(2.*fract(10.*q.yx)-1.); 
		float d = .6*max(s.x-s.y,s.x+s.y)+max(s.x,s.y)-.01;
		float edge = .005+.05*min(.5*abs(fi-5.-dof),1.);
		acc += vec3(smoothstep(edge,-edge,d)*(r.x/(1.+.02*fi*DEPTH)));
	}



  return acc;
}

const float GOLDEN_ANGLE = 2.39996323;
const float MAX_BLUR_SIZE = 20.0;
const float RAD_SCALE = 2.0; // Smaller = nicer blur, larger = faster


// Cherno Depth of Field
float GetBlurSize(float depth, float focusPoint, float focusScale)
{
	float coc = clamp((1.0 / focusPoint - 1.0 / depth) * focusScale, -1.0, 1.0);
	return abs(coc) * MAX_BLUR_SIZE;
}


vec3 DepthOfField(vec3 color, vec2 texCoord, float focusPoint, float focusScale, vec2 texelSize)
{
	float centerDepth = LinearizeDepth(texture(sceneDepthMap, texCoord).r, 0.1, FarPlane);// *uFar;
	float centerSize = GetBlurSize(centerDepth, focusPoint, focusScale);
	float tot = 1.0;
	float radius = RAD_SCALE;
	for (float ang = 0.0; radius < MAX_BLUR_SIZE; ang += GOLDEN_ANGLE)
	{
		vec2 tc = texCoord + vec2(cos(ang), sin(ang)) * texelSize * radius;
		vec3 sampleColor = texture(sceneColorMap, tc).rgb;
		float sampleDepth = texture(sceneDepthMap, tc).r * FarPlane;
		float sampleSize = GetBlurSize(sampleDepth, focusPoint, focusScale);
		if (sampleDepth > centerDepth)
			sampleSize = clamp(sampleSize, 0.0, centerSize * 2.0);
		float m = smoothstep(radius - 0.5, radius + 0.5, sampleSize);
		color += mix(color / tot, sampleColor, m);
		tot += 1.0;
		radius += RAD_SCALE / radius;
	}
	return color /= tot;
}
