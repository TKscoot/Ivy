#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D sceneColorMap;
layout(binding = 1) uniform sampler2D sceneDepthMap;
layout(binding = 2) uniform sampler2D godrayOcclusionMap;

uniform mat4 World;		
uniform mat4 View;						
uniform mat4 Projection;				
uniform mat4 WorldViewProjection;		
uniform mat4 PreviousWorldViewProjection;
uniform mat4 InverseViewProjection;	
uniform vec2 WindowResolution;	
uniform int  Tonemap;	
uniform bool UseDepthOfField;
uniform bool UseMotionBlur;


// Tonemapping implementation from shadow-swan by Tobias Arrskog (https://github.com/topfs2/shadow-swan/blob/master/shaders/tonemap.glsl)
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
        color = filmicToneMapping(color);
        break;
    case 5:
        color = Uncharted2ToneMapping(color);
        break;
	}
	return color;
}
// End of Tonemapping implementation


void DoF(inout vec3 color)
{
    float depth = texture2D(sceneDepthMap, TexCoords).r;
 
 
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
	//return col.rgb;
}


vec4 motionBlur(sampler2D color, vec2 uv, float intensity)
{
    vec2 speed = vec2(0.05, 0.0);
    vec2 offset = intensity * speed;
    vec3 c = vec3(0.);
    float inc = 0.1;
    float weight = 0.;
    for (float i = 0.; i <= 1.; i += inc)
    {
        c += texture2D(color, uv + i * offset).rgb;
        weight += 1.;
    }
    c /= weight;
    return vec4(c, 1.);
}

// Motion Blur effect used and customized from GPU Gems 3 
// (https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-27-motion-blur-post-processing-effect)
void motionBlur1(inout vec3 color)
{
	vec2 texCoord = TexCoords;

	// Get the depth buffer value at this pixel.
	float zOverW = texture2D(sceneDepthMap, texCoord).r;
	
	// H is the viewport position at this pixel in the range -1 to 1.
	vec4 H = vec4(texCoord.x * 2 - 1, (1 - texCoord.y) * 2 - 1,zOverW, 1);
	
	// Transform by the view-projection inverse.
	//vec4 D = mul(H, g_ViewProjectionInverseMatrix);
	vec4 D = InverseViewProjection * H;
	
	// Divide by w to get the world position.
	vec4 worldPos = D / D.w;

	// Current viewport position
	vec4 currentPos = H;
	
	// Use the world position, and transform by the previous view-
	// projection matrix.
	//vec4 previousPos = mul(worldPos, g_previousViewProjectionMatrix);
	vec4 previousPos = PreviousWorldViewProjection * worldPos;
	
	// Convert to nonhomogeneous points [-1,1] by dividing by w.
	previousPos /= previousPos.w;
	
	// Use this frame's position and last frame's to compute the pixel
	// velocity.
	vec4 vel = (currentPos - previousPos)/2. * 0.05;

	// Get the initial color at this pixel.
	//color = texture2D(sceneColorMap, texCoord);
	
	float velocity = (vel.x + vel.y + vel.z) / 3.;

	texCoord += velocity;
	
	for(int i = 1; i < 10; ++i, texCoord += velocity)
	{
	  // Sample the color buffer along the velocity vector.
	  vec4 currentColor = texture2D(sceneColorMap, texCoord);
	
	  // Add the current color to our color sum.
	  color += currentColor.rgb;
	}
	
	// Average all of the samples to get the final blur color.
	color = color / 10;
}

  
vec3 godrays(
    float density,
    float weight,
    float decay,
    float exposure,
    int numSamples,
    sampler2D occlusionTexture,
    vec2 screenSpaceLightPos,
    vec2 uv
    ) {

    vec3 fragColor = vec3(0.0,0.0,0.0);

	vec2 deltaTextCoord = vec2( uv - screenSpaceLightPos.xy );

	vec2 textCoo = uv.xy ;
	deltaTextCoord *= (1.0 /  float(numSamples)) * density;
	float illuminationDecay = 1.0;


	for(int i=0; i < 100 ; i++){


        /*
        This makes sure that the loop only runs `numSamples` many times.
        We have to do it this way in WebGL, since you can't have a for loop
        that runs a variable number times in WebGL.
        This little hack gets around that.
        But the drawback of this is that we have to specify an upper bound to the
        number of iterations(but 100 is good enough for almost all cases.)
        */
	    if(numSamples < i) {
            break;
	    }

		textCoo -= deltaTextCoord;
		vec3 samp = texture2D(occlusionTexture, textCoo   ).xyz;
		samp *= illuminationDecay * weight;
		fragColor += samp;
		illuminationDecay *= decay;
	}

	fragColor *= exposure;

    return fragColor;


}

void main()
{
	const float gamma     = 2.2;
	const float pureWhite = 1.0;
	
	vec3 color = texture2D(sceneColorMap, TexCoords).rgb;
	float depth = texture2D(sceneDepthMap, TexCoords).r;



	// WORKING BLUR (NOT MOTION BLUR!) From https://www.shadertoy.com/view/Xltfzj

	//float Pi = 6.28318530718; // Pi*2
    //
    //// GAUSSIAN BLUR SETTINGS {{{
    //float Directions =32.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    //float Quality = 6.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    //float Size = 16.0; // BLUR SIZE (Radius)
    //// GAUSSIAN BLUR SETTINGS }}}
   	//
    //vec2 Radius = Size/textureSize(sceneColorMap, 0);
	//
    //vec3 result = texture(sceneColorMap, TexCoords).rgb;
    //vec2 uv = TexCoords;
    //// Blur calculations
    //for( float d=0.0; d<Pi; d+=Pi/Directions)
    //{
	//	for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)
    //    {
	//		result += texture( sceneColorMap, uv+vec2(cos(d),sin(d))*Radius*i).rgb;
    //    }
    //}
    //
    //result /= Quality * Directions - 15.0;
	//
	//color = result;

	if(UseMotionBlur)
	{
		motionBlur1(color);
	}
	if(UseDepthOfField)
	{
		DoF(color);
    }
	//color += mb + dof;

	//vec3 fragColor = godrays(
    //uDensity,
    //uWeight,
    //uDecay,
    //uExposure,
    //uNumSamples,
    //uOcclusionTexture,
    //uScreenSpaceSunPos,
    //vUv
    //);

	vec4 sunPos = Projection * View * vec4(-2.0, 4.0, -1.0, 1.0);
	vec2 screenSpaceSunPos = vec2(0.0);
	screenSpaceSunPos.x = sunPos.x / sunPos.w;
	screenSpaceSunPos.y = sunPos.y / sunPos.w;

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
	
	// gamma correct
    color = pow(color, vec3(1.0/2.2)); 
	
	vec3 mappedColor = tonemapAuto(color);

	// Final color output!
	//FragColor = vec4(mappedColor, 1.0f);

	FragColor = vec4(mappedColor, 1.0f);
}