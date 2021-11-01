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
uniform float ElapsedTime;

out vec3 FragPos;
out vec3 ViewPosition;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;
out vec4 FragPosLightSpace[4];
out mat4 Projection;
out mat4 View;

float strength = 0.01;
float speed = 1.0;
float deforming = 0.0;
float bending = 0.0;
float ground = 0.0;
vec2 direction = vec2(0, 1);

// ########################################Settings########################################

vec2 windDirection = direction; // Direction of the wind
float windStrength = strength;  // Strength of the wind
float groundLevel = ground;     // Everything over ground level will have movement
const int windSamples = 3;      // Number of wind sine waves to add
// ########################################Functions########################################


vec2 getWind(vec4 worldPos, float height)
// Function to generate the wind
{
    vec2 wind;
    if (height > groundLevel)
    {
        float windTime = ElapsedTime*speed;
        float windDisplacement = cos(0.375*((17.5+worldPos.x)+(17.5+worldPos.y))+(windTime*1.25));
        
        for (int w = 0; w < windSamples; w++)
        {
            float rAnd = float(w)+1.0-(float(windSamples)/2.0);
            float rCnd = float(w)-1.0+(float(windSamples)/2.0);
            windDisplacement += sin(0.5*((17.5+rAnd+worldPos.x)+(rAnd+worldPos.y))+(windTime*(rAnd*0.1+1.75)));
            windDisplacement -= cos(0.5*((17.5+rCnd+worldPos.x)+(rAnd+worldPos.y))+(windTime*(rCnd*0.1+1.75)));
        }

        wind = windStrength*(height-0.25)*sin((worldPos.xy*normalize(windDirection))+vec2(windTime*0.5))*windDisplacement;
        wind *= ((height - groundLevel) * bending + 1.0 - bending);
    }
    else
    {
        wind = vec2(0.0,0.0);        
    }
    return wind;
    
}

mat4 addWorldPos(mat4 modelMat, vec3 pos)
{
   modelMat[3][0] += pos.x;
   modelMat[3][1] += pos.y;
   modelMat[3][2] += pos.z;   
   return modelMat;
}

void main()
{
    FragPos = vec3(model * vec4(aPosition.xyz, 1.0));   
	vec2 wind = getWind(vec4(FragPos.xyz, 1.0), aPosition.y);
    TexCoords = aTexCoord;
	ViewPosition = vec3(view * vec4(FragPos, 1.0));
	Projection = projection;

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
        
    gl_Position = projection * view * model /*addWorldPos(model, vec3(wind, 0.0))*/ * vec4(aPosition.xyz, 1.0);
} 
