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

Ivy::Shader::Shader(const String & computeFilepath)
{
	if(computeFilepath.find(".glsl") != std::string::npos ||
		computeFilepath.find(".comp") != std::string::npos)
	{
		sources[GL_COMPUTE_SHADER] = ReadFile(computeFilepath);
	}
	else
	{
		Debug::CoreError("Vertex shader file extension not supported. Use .glsl or .vert ({})", computeFilepath);
		return;
	}


	Compile();
}

Ivy::Shader::~Shader()
{
	glDeleteProgram(mProgram);
}

void Ivy::Shader::Bind()
{
	glUseProgram(mProgram);
}

void Ivy::Shader::Unbind()
{
	glUseProgram(0);
}

void Ivy::Shader::SetUniformInt(const String & name, int value)
{
	GLint location = glGetUniformLocation(mProgram, name.c_str());
	glUniform1i(location, value);
}

void Ivy::Shader::SetUniformIntArray(const String & name, int * values, uint32_t count)
{
	GLint location = glGetUniformLocation(mProgram, name.c_str());
	glUniform1iv(location, count, values);
}

void Ivy::Shader::SetUniformFloat(const String & name, float value)
{
	GLint location = glGetUniformLocation(mProgram, name.c_str());
	glUniform1f(location, value);
}

void Ivy::Shader::SetUniformFloat2(const String & name, const Vec2 & value)
{
	GLint location = glGetUniformLocation(mProgram, name.c_str());
	glUniform2f(location, value.x, value.y);
}

void Ivy::Shader::SetUniformFloat3(const String & name, const Vec3 & value)
{
	GLint location = glGetUniformLocation(mProgram, name.c_str());
	glUniform3f(location, value.x, value.y, value.z);
}

void Ivy::Shader::SetUniformFloat4(const String & name, const Vec4 & value)
{
	GLint location = glGetUniformLocation(mProgram, name.c_str());
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Ivy::Shader::SetUniformMat3(const String & name, const Mat3 & matrix)
{
	GLint location = glGetUniformLocation(mProgram, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Ivy::Shader::SetUniformMat4(const String & name, const Mat4 & matrix)
{
	GLint location = glGetUniformLocation(mProgram, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

uint32_t Ivy::Shader::GetCurrentlyUsedShaderID()
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);

	return static_cast<uint32_t>(id);
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

	std::array<GLuint, 2> shaderIDs = { 0, 0 };

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
		if(id != 0)
		{
			glDetachShader(mProgram, id);
			glDeleteShader(id);
		}
	}
}
