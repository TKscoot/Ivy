#pragma once
#include "Types.h"
#include "Component.h"
#include "rendering/Shader.h"
#include "rendering/Texture.h"

namespace Ivy
{
	/*!
	 * Material component
	 * Handles the shader, textures, pbr values and ibl
	 * 
	 */
	class Material : public Component
	{
	public:
		/*!
		 * Types of textures
		 * 
		 */
		enum class TextureMapType
		{
			DIFFUSE = 0,
			NORMAL = 1,
			ROUGHNESS = 2,
			METALLIC = 3,
			BRDF_LUT = 4,

			TEXTURE_MAP_TYPE_LAST
		};

		/*!
		 * Default constructor
		 * 
		 */
		Material();

		/*!
		 * Sets a texture object as specified texture type
		 * 
		 * \param texture Pointer to the texture
		 * \param type Type the texture should be used
		 */
		void SetTexture(Ptr<Texture2D> texture, TextureMapType type) { mTextures[type] = texture; }

		/*!
		 * Loads a texture from a file as specified texture type
		 * 
		 * \param texturePath The path to the texture image
		 * \param type Type the texture should be used
		 * \return 
		 */
		Ivy::Ptr<Texture2D> LoadTexture(String texturePath, TextureMapType type);

		/*!
		 * Defines the shader which should be used by this material
		 * 
		 * \param shader Pointer to the shader
		 */
		void SetShader(Ptr<Shader> shader)
		{
			mShader = shader;
			if(!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}

			// Set default color values
			mShader->Bind();
			SetTextureTiling(mTextureTiling);
			SetAmbientColor(mAmbient);
			SetDiffuseColor(mDiffuse);
			SetSpecularColor(mSpecular);
			SetMetallic(mMetallic);
			SetRoughness(mRoughness);
			UseIBL(mUseIBL);
			SetDefaultShaderUniforms();
			mShader->Unbind();
		}

		/*!
		 * Loads and uses the specified shader from shader files
		 * 
		 * \param vertexPath Filepath to the vertexshader
		 * \param fragmentPath Filepath to the fragmentshader
		 */
		void LoadShader(String vertexPath, String fragmentPath);

		/*!
		 * 
		 * \return Returns the Pointer to the shader object
		 */
		Ptr<Shader> GetShader() { return mShader; }

		/*!
		 * 
		 * \return Returns all textures with their respective texture type
		 */
		UnorderedMap<TextureMapType, Ptr<Texture2D>> GetTextures() { return mTextures; }

		/*!
		 * Sets the tiling of the texture. Default tiling is x & y = 1.0f
		 * 
		 * \param tiling X and Y tiling of the texture
		 */
		void SetTextureTiling(Vec2 tiling)
		{
			mTextureTiling = tiling;
		}

		/*!
		 * 
		 * \param colorValue Ambient color to use
		 */
		void SetAmbientColor(Vec3 colorValue)
		{
			mAmbient = colorValue;
			if(!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->Bind();
			mShader->SetUniformFloat3("material.ambient", colorValue);
			mShader->Unbind();
		}

		/*!
		 *
		 * \param colorValue Diffuse color to use
		 */
		void SetDiffuseColor(Vec3 colorValue)
		{
			mDiffuse = colorValue;
			if(!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->Bind();
			mShader->SetUniformFloat3("material.diffuse", colorValue);
			mShader->Unbind();
		}

		/*!
		 *
		 * \param colorValue Specular color to use
		 */
		void SetSpecularColor(Vec3 colorValue)
		{
			mSpecular = colorValue;
			if(!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->Bind();
			mShader->SetUniformFloat3("material.specular", colorValue);
			mShader->Unbind();
		}

		/*!
		 * 
		 * \param factor Metallic factor
		 */
		void SetMetallic(float factor)
		{
			mMetallic = factor;
			if(!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->Bind();
			mShader->SetUniformFloat("material.metallic", factor);
			mShader->Unbind();
		}

		/*!
		 * 
		 * \param factor Roughness factor
		 */
		void SetRoughness(float factor)
		{
			mRoughness = factor;
			if(!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->Bind();
			mShader->SetUniformFloat("material.roughness", factor);
			mShader->Unbind();
		}

		/*!
		 * 
		 * \param value Bool if image based lighting should be used
		 */
		void UseIBL(bool value)
		{
			mUseIBL = value;
			if(!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->Bind();
			mShader->SetUniformInt("useIBL", (int)value);
			mShader->Unbind();
		}

		/*!
		 * Internal
		 * Updates if texture type should be used
		 * 
		 */
		void UpdateShaderTextureBools();

		/*!
		 * Updates and uploads the material parameters
		 * 
		 */
		void UpdateMaterialUniforms();

	private:
		void SetDefaultShaderUniforms();

		UnorderedMap<TextureMapType, Ptr<Texture2D>> mTextures;
		static UnorderedMap<String, Ivy::Ptr<Texture2D>> mLoadedTextures;

		std::array<bool, (size_t)TextureMapType::TEXTURE_MAP_TYPE_LAST> mUsedTextureTypes;

		Ptr<Shader> mShader = nullptr;

		Vec3  mAmbient		 = Vec3(0.5f, 0.5f, 0.5f);
		Vec3  mDiffuse		 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3  mSpecular		 = Vec3(0.5f, 0.5f, 0.5f);
		float mMetallic		 = 0.1f;
		float mRoughness	 = 0.9f;
		bool  mUseIBL		 = true;
		Vec2  mTextureTiling = Vec2(1.0f, 1.0f);

	};
}

