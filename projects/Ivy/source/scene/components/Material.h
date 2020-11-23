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

			BRDF_LUT = 5
		};

		Material();

		void SetTexture(Ptr<Texture2D> texture, TextureMapType type) { mTextures[type] = texture; }
		Ivy::Ptr<Texture2D> LoadTexture(String texturePath, TextureMapType type);

		void SetShader(Ptr<Shader> shader) 
		{
			mShader = shader; 
			if (!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
		
			// Set default color values
			mShader->Bind();
			SetAmbientColor(mAmbient);
			SetDiffuseColor(mDiffuse);
			SetSpecularColor(mSpecular);
			SetShininess(mShininess);
			mShader->Unbind();
		}
		
		void LoadShader(String vertexPath, String fragmentPath); // TODO: Implement

		Ptr<Shader> GetShader() { return mShader; }
		UnorderedMap<TextureMapType, Ptr<Texture2D>> GetTextures() { return mTextures; }

		void SetAmbientColor (Vec3 colorValue) 
		{ 
			mAmbient  = colorValue; 
			if (!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->Bind();
			mShader->SetUniformFloat3("material.ambient", colorValue);
			mShader->Unbind();
		}
		void SetDiffuseColor (Vec3 colorValue) 
		{ 
			mDiffuse  = colorValue; 
			if (!mShader)
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
			if (!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->Bind();
			mShader->SetUniformFloat3("material.specular", colorValue);
			mShader->Unbind();
		}
		void SetShininess(float factor) 
		{ 
			mShininess = factor; 
			if (!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->Bind();
			mShader->SetUniformFloat("material.shininess", factor);
			mShader->Unbind();
		}

	private:
		void SetDefaultShaderUniforms();
		
		UnorderedMap<TextureMapType, Ptr<Texture2D>> mTextures;
		static UnorderedMap<String, Ivy::Ptr<Texture2D>> mLoadedTextures;

		Ptr<Shader> mShader = nullptr;

		Vec3  mAmbient   = Vec3(0.5f, 0.5f, 0.5f);
		Vec3  mDiffuse   = Vec3(1.0f, 1.0f, 1.0f);
		Vec3  mSpecular  = Vec3(0.5f, 0.5f, 0.5f);
		float mShininess = 32.0f;

	};
}

