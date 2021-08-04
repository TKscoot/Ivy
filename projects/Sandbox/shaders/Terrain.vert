
#version 450 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec3 FragPos;
out vec3 ViewPosition;
out vec2 TexCoords;
out vec3 Normal;
out vec3 Halfway;

const vec3 light = normalize (vec3 (2, 1, 3));

void main()
{
    FragPos = vec3(model * vec4(aPosition.xyz, 1.0));   
    TexCoords = aTexCoord;
	ViewPosition = vec3(view * vec4(FragPos, 1.0));

	Normal = mat3(model) * aNormal;
	Halfway = normalize(ViewPosition + light);

    gl_Position = projection * view * model * vec4(aPosition.xyz, 1.0);

}
		