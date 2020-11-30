#version 450 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix0;
uniform mat4 lightSpaceMatrix1;
uniform mat4 lightSpaceMatrix2;
uniform mat4 lightSpaceMatrix3;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 ViewPosition;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;
out vec4 FragPosLightSpace[4];

void main()
{
    FragPos = vec3(model * vec4(aPosition.xyz, 1.0));   
    TexCoords = aTexCoord;
	ViewPosition = vec3(view * vec4(FragPos, 1.0));

	mat3 modelVector = transpose(inverse(mat3(model)));
    
	vec3 T = normalize(modelVector * aTangent);
	vec3 B = normalize(modelVector * aBitangent);
	vec3 N = normalize(modelVector * aNormal);
	TBN = mat3(T, B, N); 

	//Normal = normalize(modelVector * aNormal);
	Normal = mat3(model) * aNormal;

	FragPosLightSpace[0] = lightSpaceMatrix0 * vec4(FragPos, 1.0);
	FragPosLightSpace[1] = lightSpaceMatrix1 * vec4(FragPos, 1.0);
	FragPosLightSpace[2] = lightSpaceMatrix2 * vec4(FragPos, 1.0);
	FragPosLightSpace[3] = lightSpaceMatrix3 * vec4(FragPos, 1.0);
	//FragPosLightSpace = FragPosLightSpace * vec4(viewPos, 1.0);
        
    gl_Position = projection * view * model * vec4(aPosition.xyz, 1.0);

} 
