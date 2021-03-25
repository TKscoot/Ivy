#include "ivypch.h"
#include "Line.h"

Ivy::Line::Line(Vec3 start, Vec3 end)
{
	mStartPoint = start;
	mEndPoint = end;
	mLineColor = Vec3(1, 1, 1);
	mLineWidth = 1.0f;

	const char *vertexShaderSource =
		"#version 450 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"uniform mat4 MVP;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char *fragmentShaderSource =
		"#version 450 core\n"
		"out vec4 FragColor;\n"
		"uniform vec3 color;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(color, 1.0f);\n"
		"}\n\0";

	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors

	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors

	// link shaders
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, vertexShader);
	glAttachShader(mShaderProgram, fragmentShader);
	glLinkProgram( mShaderProgram);
	// check for linking errors

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	mVertices = {
		 start.x, start.y, start.z,
		 end.x, end.y, end.z,

	};

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
