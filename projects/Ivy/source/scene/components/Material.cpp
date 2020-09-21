#include "ivypch.h"
#include "Material.h"

Ivy::Material::Material()
{
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

void Ivy::Material::LoadTexture(String texturePath, TextureMapType type)
{
	if (texturePath.empty())
	{
		Debug::CoreWarning("Path to texture is empty. skipping texture loading! ({})", texturePath);
		return;
	}
	mTextures[type] = CreatePtr<Texture2D>(texturePath);
}

void Ivy::Material::LoadShader(String vertexPath, String fragmentPath)
{
}
