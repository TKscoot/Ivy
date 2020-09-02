#pragma once
#include <Types.h>
#include <environment/Log.h>

namespace Ivy
{
	class Shader
	{
	public:
		Shader(const String vertexFilepath, const String fragmentFilepath);
		~Shader();

		///
		/// Binds the shader program. This tells the renderer to use this program.
		///
		void Bind();
		///
		/// Unbinds the shader program.
		///
		void Unbind();

		///
		/// 
		///
		void SetUniformInt(		const String& name, int value);
		///
		///
		///
		void SetUniformIntArray(const String& name, int* values, uint32_t count);

		///
		///
		///
		void SetUniformFloat(	const String& name, float value);
		///
		///
		///
		void SetUniformFloat2(	const String& name, const Vec2& value);
		///
		///
		///
		void SetUniformFloat3(	const String& name, const Vec3& value);
		///
		///
		///
		void SetUniformFloat4(	const String& name, const Vec4& value);

		///
		///
		///
		void SetUniformMat3(	const String& name, const Mat3& matrix);
		///
		///
		///
		void SetUniformMat4(	const String& name, const Mat4& matrix);

	private:
		// Methods
		String ReadFile(const String filepath);
		void Compile();

		// Variables
		UnorderedMap<GLuint, String> sources;
		GLuint mProgram = 0;

	};
}