#pragma once
#include "Types.h"
#include "Component.h"
#include "rendering/Shader.h"
#include "rendering/Texture.h"

namespace Ivy
{
	class Material : public Component
	{
	public:
		enum class TextureMapType
		{
			DIFFUSE = 0,
			NORMAL = 1,
			ROUGHNESS = 2,
			METALLIC = 3,
			AMBIENT_OCCLUSION = 4,

			BRDF_LUT = 5,

			TEXTURE_MAP_TYPE_LAST
		};

		Material();

		void SetTexture(Ptr<Texture2D> texture, TextureMapType type) { mTextures[type] = texture; }
		Ivy::Ptr<Texture2D> LoadTexture(String texturePath, TextureMapType type);

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
			mShader->Unbind();
		}

		void LoadShader(String vertexPath, String fragmentPath); // TODO: Implement

		Ptr<Shader> GetShader() { return mShader; }
		UnorderedMap<TextureMapType, Ptr<Texture2D>> GetTextures() { return mTextures; }

		void SetTextureTiling(Vec2 tiling)
		{
			mTextureTiling = tiling;
		}

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

		void UpdateShaderTextureBools();
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
		float mMetallic		 = 0.001f;
		float mRoughness	 = 1.0f;
		bool  mUseIBL		 = true;
		Vec2  mTextureTiling = Vec2(1.0f, 1.0f);

	};
}

