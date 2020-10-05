#include "ivypch.h"
#include "Material.h"

Ivy::UnorderedMap<Ivy::String, Ivy::Ptr<Ivy::Texture2D>> Ivy::Material::mLoadedTextures = {};

Ivy::Material::Material()
	: mAmbient(Vec3(0.5f, 0.5f, 0.5f))
	, mDiffuse(Vec3(1.0f, 1.0f, 1.0f))
	, mSpecular(Vec3(0.5f, 0.5f, 0.5f))
	, mShininess(32.0f)
{
	// Set default shader
	static Ptr<Shader> defaultShader = CreatePtr<Shader>("shaders/Triangle.vert", "shaders/Triangle.frag");
	mShader = defaultShader;
	// Set default color values
	mShader->Bind();
	SetAmbientColor(mAmbient);
	SetDiffuseColor(mDiffuse);
	SetSpecularColor(mSpecular);
	SetShininess(mShininess);
	mShader->Unbind();

	int width  = 2;
	int height = 2;
	unsigned int arraySize = width * height * 4/*channels*/;
	GLubyte* pixelData = new GLubyte[arraySize];

	// Set all pixels pink & full opacity
	for (int i = 0; i < width * height; ++i)
	{
		pixelData[i * 4 + 0] = 255;
		pixelData[i * 4 + 1] = 0;
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
	mTextures[TextureMapType::NORMAL]	 = CreatePtr<Texture2D>(2, 2, pixelData);

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
	
	return mTextures[type];
}

void Ivy::Material::LoadShader(String vertexPath, String fragmentPath)
{
}
