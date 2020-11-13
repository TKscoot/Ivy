#version 330 core
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTexCoord;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(aPosition.xyz, 1.0);
}
