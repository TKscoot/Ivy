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
		enum TextureMapType
		{
			DIFFUSE,
			NORMAL,
			ROUGHNESS,
			METALLIC
		};

		Material();

		void SetTexture(Ptr<Texture2D> texture, TextureMapType type) { mTextures[type] = texture; }
		void LoadTexture(String texturePath, TextureMapType type);

		void SetShader(Ptr<Shader> shader) { mShader = shader; }
		void LoadShader(String vertexPath, String fragmentPath);

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
			mShader->SetUniformFloat3("material.ambient", colorValue);
		}
		void SetDiffuseColor (Vec3 colorValue) 
		{ 
			mDiffuse  = colorValue; 
			if (!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->SetUniformFloat3("material.diffuse", colorValue);

		}
		void SetSpecularColor(Vec3 colorValue) 
		{ 
			mSpecular = colorValue; 
			if (!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->SetUniformFloat3("material.specular", colorValue);

		}
		void SetShininess(float factor) 
		{ 
			mShininess = factor; 
			if (!mShader)
			{
				Debug::CoreError("Shader is null!");
				return;
			}
			mShader->SetUniformFloat("material.shininess", factor);
		}

	private:
		UnorderedMap<TextureMapType, Ptr<Texture2D>> mTextures;

		Ptr<Shader>	   mShader = nullptr;

		Vec3  mAmbient;
		Vec3  mDiffuse;
		Vec3  mSpecular;
		float mShininess;

	};
}

