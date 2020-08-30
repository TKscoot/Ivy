#include "ivypch.h"
#include "Shader.h"

Ivy::Shader::Shader(const String vertexFilepath, const String fragmentFilepath)
{
	if (vertexFilepath.find(".glsl") != std::string::npos ||
		vertexFilepath.find(".vert") != std::string::npos)
	{
		sources[GL_VERTEX_SHADER]   = ReadFile(vertexFilepath);
	}
	else
	{
		Debug::CoreError("Vertex shader file extension not supported. Use .glsl or .vert ({})", vertexFilepath);
		return;
	}

	if (fragmentFilepath.find(".glsl") != std::string::npos ||
		fragmentFilepath.find(".frag") != std::string::npos)
	{
		sources[GL_FRAGMENT_SHADER] = ReadFile(fragmentFilepath);
	}
	else
	{
		Debug::CoreError("Fragment shader file extension not supported. Use .glsl or .frag ({})", fragmentFilepath);
		return;
	}


	Compile();
}

void Ivy::Shader::Bind()
{
	glUseProgram(mProgram);
}

void Ivy::Shader::Unbind()
{
	glUseProgram(0);
}

Ivy::String Ivy::Shader::ReadFile(const String filepath)
{
	String result = "";

	// Open file
	std::ifstream fs(filepath.c_str());
	if (!fs)
	{
		Debug::CoreError("Could not read shaderfile: {}", filepath);
	}

	// Iterate through file and save result in string
	result.assign((std::istreambuf_iterator<char>(fs)),
				  (std::istreambuf_iterator<char>()));


	return result;
}

void Ivy::Shader::Compile()
{
	mProgram = glCreateProgram();

	std::array<GLuint, 2> shaderIDs;

	// Debugging vars
	int  success;
	char infoLog[512];

	int i = 0;
	for (auto& kv : sources)
	{
		GLuint type = kv.first;
		const GLchar* source = kv.second.c_str();

		GLuint shader = glCreateShader(type);


		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);


		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			Debug::CoreError("ERROR::SHADER::COMPILATION_FAILED | {}", infoLog);
		}

		glAttachShader(mProgram, shader);
		shaderIDs[i] = shader;
		i++;
	}

	glLinkProgram(mProgram);

	glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(mProgram, 512, NULL, infoLog);
		Debug::CoreError("ERROR::SHADER::PROGRAM::LINKING_FAILED | {}", infoLog);
	}

	for (auto id : shaderIDs)
	{
		glDetachShader(mProgram, id);
		glDeleteShader(id);
	}
}
