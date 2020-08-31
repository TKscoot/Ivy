#version 450 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 color;
out vec2 texCoord;

void main()
{
	gl_Position = vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
	color = aColor;
	texCoord = aTexCoord;
} 