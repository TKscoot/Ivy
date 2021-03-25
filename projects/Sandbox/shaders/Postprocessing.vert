#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aCoords;

out vec2 TexCoords;
out vec2 Pos;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	TexCoords = aCoords;
	Pos = aPos;
}
