#version 450 core
 
out vec4 FragColor;

out vec3 normal;
out vec3 tangent;
in vec2 texCoord;
uniform vec4 aColor;

layout(binding = 0) uniform sampler2D albedo;
layout(binding = 1) uniform sampler2D albedo1;

void main()
{
	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	//FragColor = color * aColor;
	FragColor = mix(texture(albedo, texCoord), texture(albedo1, texCoord), 0.2);
}