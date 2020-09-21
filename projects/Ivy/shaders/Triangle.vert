#version 450 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 tangent;
out vec2 texCoord;

void main()
{
	//gl_Position = vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
        gl_Position = projection * view * model * aPosition;
	//color = aColor;
	normal = aNormal;
	tangent = aTangent;
	texCoord = aTexCoord;
} 
