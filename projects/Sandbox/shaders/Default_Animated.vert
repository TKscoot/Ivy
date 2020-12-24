#version 450 core

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
uniform mat4 lightSpaceMatrix0;
uniform mat4 lightSpaceMatrix1;
uniform mat4 lightSpaceMatrix2;
uniform mat4 lightSpaceMatrix3;


uniform mat4 boneTransforms[100];

out vec3 FragPos;
out vec3 ViewPosition;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;
out vec4 FragPosLightSpace[4];

void main()
{
    mat4 boneTransform  = boneTransforms[aBoneIDs[0]] * aBoneWeights[0];
		 boneTransform += boneTransforms[aBoneIDs[1]] * aBoneWeights[1];
		 boneTransform += boneTransforms[aBoneIDs[2]] * aBoneWeights[2];
		 boneTransform += boneTransforms[aBoneIDs[3]] * aBoneWeights[3];

	vec4 localPosition = boneTransform * vec4(aPosition.xyz, 1.0);

    FragPos = vec3(model * localPosition);   
    TexCoords = aTexCoord;
	ViewPosition = vec3(view * vec4(FragPos, 1.0));

	mat3 modelVector = transpose(inverse(mat3(model)));
    
	vec3 T = normalize(modelVector * (aTangent	 * mat3(boneTransform)));
	vec3 B = normalize(modelVector * (aBitangent * mat3(boneTransform)));
	vec3 N = normalize(modelVector * (aNormal	 * mat3(boneTransform)));
	TBN = mat3(T, B, N); 

	Normal = mat3(model) * mat3(boneTransform) * aNormal;

	FragPosLightSpace[0] = lightSpaceMatrix0 * vec4(FragPos, 1.0);
	FragPosLightSpace[1] = lightSpaceMatrix1 * vec4(FragPos, 1.0);
	FragPosLightSpace[2] = lightSpaceMatrix2 * vec4(FragPos, 1.0);
	FragPosLightSpace[3] = lightSpaceMatrix3 * vec4(FragPos, 1.0);
        
    gl_Position = projection * view * model * localPosition;

} 
