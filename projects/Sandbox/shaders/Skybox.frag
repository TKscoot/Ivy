#version 450 core
#extension GL_ARB_shading_language_include : require

#include "/Cloud_Functions.glsl"
#include "/Atmospheric_Scattering_Functions.glsl"
out vec4 FragColor;

in vec3 TexCoords;
in vec3 pos;

layout(binding = 0) uniform samplerCube skybox;

uniform vec3 sunPosition;
uniform vec3 Direction;


uniform bool calculateClouds = false;
uniform float time    = 0.0;
uniform float cirrus  = 0.4;
uniform float cumulus = 0.8;

const float Br = 0.0025;
const float Bm = 0.0003;
const float g =  0.9800;
const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
const vec3 Km = Bm / pow(nitrogen, vec3(0.84));






  

void main()
{
	vec3 color = texture(skybox, TexCoords).rgb;

	if(calculateClouds)
	{
		vec3 day_extinction = vec3(1.0);// exp(-exp(-((pos.y + sunPosition.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / Br) * (exp(-pos.y * 16.0) + 0.1) * Kr / Br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0;
		vec3 night_extinction = vec3(1.0 - exp(sunPosition.y)) * 0.2;
		vec3 extinction = mix(day_extinction, night_extinction, -sunPosition.y * 0.2 + 0.5);

		float cirrusTimeMul = 0.001;
		float cumulusTimeMul = 0.01;

		// Cirrus Clouds
		float density = smoothstep(1.0 - cirrus, 1.0, fbm(pos.xyz / pos.y * 2.0 + time * cirrusTimeMul)) * 0.3;
		color.rgb = mix(color.rgb, extinction * 4.0, density * max(pos.y, 0.0));

		// Cumulus Clouds
		for (int i = 0; i < 3; i++)
		{
		  float density = smoothstep(1.0 - cumulus, 1.0, fbm((0.7 + float(i) * 0.01) * pos.xyz / pos.y + time * cumulusTimeMul));
		  color.rgb = mix(color.rgb, extinction * density * 5.0, min(density, 1.0) * max(pos.y, 0.0));
		}
	}

	FragColor = vec4(color, 1.0);
}
