#include "ivypch.h"
#include "Material.h"

Ivy::UnorderedMap<Ivy::String, Ivy::Ptr<Ivy::Texture2D>> Ivy::Material::mLoadedTextures = {};

Ivy::Material::Material()
	: mAmbient(Vec3(0.5f, 0.5f, 0.5f))
	, mDiffuse(Vec3(1.0f, 1.0f, 1.0f))
	, mSpecular(Vec3(0.5f, 0.5f, 0.5f))
{
	// Set default shader
	static Ptr<Shader> defaultShader = CreatePtr<Shader>("shaders/Default.vert", "shaders/PBR.frag");
	mShader = defaultShader;
	// Set default color values
	SetAmbientColor(mAmbient);
	SetDiffuseColor(mDiffuse);
	SetSpecularColor(mSpecular);
	SetMetallic(0.5f);
	SetRoughness(0.5f);
	UseIBL(mUseIBL);
	SetDefaultShaderUniforms();

	int width  = 2;
	int height = 2;
	unsigned int arraySize = width * height * 4/*channels*/;
	GLubyte* pixelData = new GLubyte[arraySize];

	// Set all pixels pink & full opacity
	for (int i = 0; i < width * height; ++i)
	{
		pixelData[i * 4 + 0] = 255;
		pixelData[i * 4 + 1] = 255;
		pixelData[i * 4 + 2] = 255;
		pixelData[i * 4 + 3] = 255;
	}
	mTextures[TextureMapType::DIFFUSE]   = CreatePtr<Texture2D>(2, 2, pixelData);
	
	// Set all pixels white & full opacity
	std::memset(pixelData, 255, arraySize);
	mTextures[TextureMapType::ROUGHNESS] = CreatePtr<Texture2D>(2, 2, pixelData);

	// Set all pixels black & full opacity
	for (int i = 0; i < width * height; ++i)
	{
		pixelData[i * 4 + 0] = 0;
		pixelData[i * 4 + 1] = 0;
		pixelData[i * 4 + 2] = 0;
		pixelData[i * 4 + 3] = 255;
	}
	mTextures[TextureMapType::METALLIC]  = CreatePtr<Texture2D>(2, 2, pixelData);
	// Set all pixels blue & full opacity
	for (int i = 0; i < width * height; ++i)
	{
		pixelData[i * 4 + 0] = 127;
		pixelData[i * 4 + 1] = 127;
		pixelData[i * 4 + 2] = 255;
		pixelData[i * 4 + 3] = 255;
	}
	mTextures[TextureMapType::NORMAL]	 = CreatePtr<Texture2D>(2, 2, pixelData);


	mTextures[TextureMapType::BRDF_LUT]	 = CreatePtr<Texture2D>("assets/textures/Misc/brdf_lut.jpg");

	mUsedTextureTypes.fill(false);
}

Ivy::Ptr<Ivy::Texture2D> Ivy::Material::LoadTexture(String texturePath, TextureMapType type)
{
	if (texturePath.empty())
	{
		Debug::CoreWarning("Path to texture is empty. skipping texture loading! ({})", texturePath);
		return nullptr;
	}

	auto itr = mLoadedTextures.find(texturePath);

	if (itr == mLoadedTextures.end())
	{
		mLoadedTextures[texturePath] = CreatePtr<Texture2D>(texturePath);
	}

	mTextures[type] = mLoadedTextures[texturePath];

	if (mShader)
	{
		mShader->Bind();

		switch (type)
		{
		case Material::TextureMapType::DIFFUSE:
			mUsedTextureTypes[(size_t)Material::TextureMapType::DIFFUSE] = true;
			break;
		case Material::TextureMapType::NORMAL:
			mUsedTextureTypes[(size_t)Material::TextureMapType::NORMAL] = true;
			mShader->SetUniformInt("useNormalMap", 1);
			break;
		case Material::TextureMapType::ROUGHNESS:
			mUsedTextureTypes[(size_t)Material::TextureMapType::ROUGHNESS] = true;
			mShader->SetUniformInt("useRoughnessMap", 1);
			break;
		case Material::TextureMapType::METALLIC:
			mUsedTextureTypes[(size_t)Material::TextureMapType::METALLIC] = true;
			mShader->SetUniformInt("useMetallicMap", 1);
			break;
		default:
			break;
		}

		mShader->Unbind();
	}
	
	return mTextures[type];
}

void Ivy::Material::LoadShader(String vertexPath, String fragmentPath)
{
	mShader = CreatePtr<Shader>(vertexPath, fragmentPath);
}

void Ivy::Material::UpdateShaderTextureBools()
{
	mShader->SetUniformInt("useNormalMap",		mUsedTextureTypes[1]);
	mShader->SetUniformInt("useRoughnessMap",	mUsedTextureTypes[2]);
	mShader->SetUniformInt("useMetallicMap",	mUsedTextureTypes[3]);
}

void Ivy::Material::SetDefaultShaderUniforms()
{
	if (!mShader)
	{
		Debug::CoreError("Shader is null!");
		return;
	}
	mShader->Bind();
	mShader->SetUniformInt("useNormalMap",	  0);
	mShader->SetUniformInt("useRoughnessMap", 0);
	mShader->SetUniformInt("useMetallicMap",  0);
	mShader->Unbind();
}
