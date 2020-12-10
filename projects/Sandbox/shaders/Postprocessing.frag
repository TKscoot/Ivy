#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D sceneColorMap;

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

void main()
{
	const float gamma     = 2.2;
	const float pureWhite = 1.0;
	
	vec3 color = texture2D(sceneColorMap, TexCoords).rgb;
	
	float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float mappedLuminance = (luminance * (1.0 + luminance / (pureWhite * pureWhite))) / (1.0 + luminance);
	
	// Scale color by ratio of average luminances.
	vec3 mappedColor = (mappedLuminance / luminance) * color;




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
    //// Output to screen
    //result /= Quality * Directions - 15.0;


	// Gamma correction.
	FragColor = vec4(mappedColor, 1.0f);
}
