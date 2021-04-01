#pragma once
#include <variant>
#include <Types.h>
#include <environment/Log.h>
#include "Texture.h"

namespace Ivy
{
	class Shader
	{
	public:
		Shader() {};
		Shader(const String& vertexFilepath, const String& fragmentFilepath);
		Shader(const String& computeFilepath);
		~Shader();

		void AddShaderSource(const String& vertexFilepath, const String& fragmentFilepath);

		///
		/// Binds the shader program. This tells the renderer to use this program.
		///
		void Bind();

		///
		/// Unbinds the shader program.
		///
		void Unbind();

		/*!
		 * Sets an int as a uniform in the shader
		 * 
		 * \param name the name of the uniform in the shader
		 * \param value the value to pass to the shader
		 */
		void SetUniformInt(		const String& name, int value);
		
		/*!
		 * Sets an int array as a uniform in the shader
		 *
		 * \param name the name of the uniform in the shader
		 * \param value the value to pass to the shader
		 */
		void SetUniformIntArray(const String& name, int* values, uint32_t count);
		
		/*!
		 * Sets an float as a uniform in the shader
		 *
		 * \param name the name of the uniform in the shader
		 * \param value the value to pass to the shader
		 */
		void SetUniformFloat(	const String& name, float value);

		/*!
		 * Sets an 2D float vector as a uniform in the shader
		 *
		 * \param name the name of the uniform in the shader
		 * \param value the value to pass to the shader
		 */
		void SetUniformFloat2(	const String& name, const Vec2& value);

		/*!
		 * Sets an 3D float vector as a uniform in the shader
		 *
		 * \param name the name of the uniform in the shader
		 * \param value the value to pass to the shader
		 */
		void SetUniformFloat3(	const String& name, const Vec3& value);

		/*!
		 * Sets an 4D float vector as a uniform in the shader
		 *
		 * \param name the name of the uniform in the shader
		 * \param value the value to pass to the shader
		 */
		void SetUniformFloat4(	const String& name, const Vec4& value);

		/*!
		 * Sets an 3x3 matrix as a uniform in the shader
		 *
		 * \param name the name of the uniform in the shader
		 * \param value the value to pass to the shader
		 */
		void SetUniformMat3(	const String& name, const Mat3& matrix);

		/*!
		 * Sets an 4x4 matrix as a uniform in the shader
		 *
		 * \param name the name of the uniform in the shader
		 * \param value the value to pass to the shader
		 */
		void SetUniformMat4(	const String& name, const Mat4& matrix);

		/*!
		 * Gets the OpenGL ID of the shader program
		 * 
		 * (Only use this if you know what you are doing!!!)
		 * 
		 * \return returns uint32_t of the ID
		 */
		uint32_t GetRendererID() { return mProgram; }

		/*!
		 * Gets the currently used shader program ID
		 * 
		 * (Only use this if you know what you are doing!!!)
		 * 
		 * \return returns uint32_t of the currently used shader program ID
		 */
		static uint32_t GetCurrentlyUsedShaderID();

	protected:
		// Methods
		String ReadFile(const String filepath);
		void Compile();

		// Variables
		UnorderedMap<GLuint, String> sources;
		GLuint mProgram = 0;

	};


	class ComputeShader : public Shader
	{
	public:
		ComputeShader(const String& computeFilepath);

		void Dispatch(uint32_t groupCountX = 1, uint32_t groupCountY = 1, uint32_t groupCountZ = 1);

	private:
		// TODO: TESTING FOR LATER WITH IMGUI: ImGui::Image((void*)my_texture_view, ImVec2(512,512));
		std::variant<Ptr<Texture2D>, Ptr<TextureCube>, Ptr<TextureHDRI>> mTexture;
	};
}