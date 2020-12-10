#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D sceneColorMap;

uniform mat4 World;		
uniform mat4 View;						
uniform mat4 Projection;				
uniform mat4 WorldViewProjection;		
uniform mat4 PreviousWorldViewProjection;
uniform mat4 InverseViewProjection;	
uniform vec2 WindowResolution;	
uniform int  Tonemap;			

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

void main()
{
	const float gamma     = 2.2;
	const float pureWhite = 1.0;
	
	vec3 color = texture2D(sceneColorMap, TexCoords).rgb;



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

    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

	vec3 mappedColor = tonemapAuto(color);

	// Final color output!
	FragColor = vec4(mappedColor, 1.0f);
}
