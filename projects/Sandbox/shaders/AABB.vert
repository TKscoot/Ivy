//
// Roberto Cano (http://www.robertocano.es)
//
#version 450 core

layout(location = 0) in vec3 in_vertex;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(in_vertex, 1.0f);
}