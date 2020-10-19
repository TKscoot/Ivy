#version 450 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 position;
out vec3 normal;
out vec3 tangent;
out vec2 texCoord;

//out vec3 tangentLightPos;
//out vec3 tangentViewPos;
//out vec3 tangentFragPos;

void main()
{
	//gl_Position = vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
    gl_Position = projection * view * model * aPosition;
	position = vec4(model * aPosition);
	normal   = aNormal;
	tangent  = aTangent;
	texCoord = aTexCoord;

	//mat3 normalMatrix = transpose(inverse(mat3(model)));
    //vec3 T = normalize(normalMatrix * aTangent);
    //vec3 N = normalize(normalMatrix * aNormal);
    //T = normalize(T - dot(T, N) * N);
    //vec3 B = cross(N, T);
    //
    //mat3 TBN = transpose(mat3(T, B, N));    
    //tangentLightPos = TBN * lightPos;
    //tangentViewPos  = TBN * viewPos;
    //tangentFragPos  = TBN * position.xyz;
} 
