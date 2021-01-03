#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D loadingScreenTexture;


// =====================MAIN=====================
void main()
{
	
	FragColor = vec4(texture2D(loadingScreenTexture, TexCoords).rgb, 1.0);
}
