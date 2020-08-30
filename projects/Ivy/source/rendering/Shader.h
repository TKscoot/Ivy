#pragma once
#include <Types.h>
#include <environment/Log.h>

namespace Ivy
{
	class Shader
	{
	public:
		Shader(const String vertexFilepath, const String fragmentFilepath);
		void Bind();
		void Unbind();

	private:
		// Methods
		String ReadFile(const String filepath);
		void Compile();

		// Variables
		UnorderedMap<GLuint, String> sources;
		GLuint mProgram = 0;

	};
}