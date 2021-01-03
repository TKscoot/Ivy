#version 330 core
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTexCoord;

layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4 aBoneWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform bool isAnimated;

const int MAX_BONES = 100;
uniform mat4 boneTransforms[MAX_BONES];

void main()
{
	if(isAnimated)
	{
		mat4 boneTransform  = boneTransforms[aBoneIDs.x] * aBoneWeights.x;
		boneTransform += boneTransforms[aBoneIDs.y] * aBoneWeights.y;
		boneTransform += boneTransforms[aBoneIDs.z] * aBoneWeights.z;
		boneTransform += boneTransforms[aBoneIDs.w] * aBoneWeights.w;

		vec4 localPosition = boneTransform * vec4(aPosition.xyz, 1.0);

		gl_Position = lightSpaceMatrix *  model * localPosition;

	}
	else
	{
		gl_Position = lightSpaceMatrix *  model * vec4(aPosition.xyz, 1.0f);
	}
}
