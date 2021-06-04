#include "ivypch.h"
#include "Material.h"

Ivy::UnorderedMap<Ivy::String, Ivy::Ptr<Ivy::Texture2D>> Ivy::Material::mLoadedTextures = {};
Ivy::Vector<std::future<Ivy::Texture2DData>> Ivy::Material::mFutures = {};

Ivy::Material::Material()
	: mAmbient(Vec3(0.5f, 0.5f, 0.5f))
	, mDiffuse(Vec3(1.0f, 1.0f, 1.0f))
	, mSpecular(Vec3(0.5f, 0.5f, 0.5f))
	//, mMetallic(0.0f)
	//, mRoughness(0.5f)
{
	// Set default shader
	static Ptr<Shader> defaultShader = CreatePtr<Shader>("shaders/Default.vert", "shaders/PBR.frag"); // TODO: Check if animated
	mShader = defaultShader;
	
	SetShader(defaultShader);
	// Set default color values
	SetTextureTiling(mTextureTiling);
	SetAmbientColor(mAmbient);
	SetDiffuseColor(mDiffuse);
	SetSpecularColor(mSpecular);
	SetMetallic(mMetallic);
	SetRoughness(mRoughness);
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
	mTextures[TextureMapType::ALBEDO]   = CreatePtr<Texture2D>(2, 2, pixelData);
	
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
		case Material::TextureMapType::ALBEDO:
			mUsedTextureTypes[(size_t)Material::TextureMapType::ALBEDO] = true;
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
	SetShader(mShader);
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



void Ivy::Material::UpdateMaterialUniforms()
{

	mShader->SetUniformFloat2("material.tiling",	mTextureTiling);
	mShader->SetUniformFloat3("material.ambient",   mAmbient);
	mShader->SetUniformFloat3("material.diffuse",   mDiffuse);
	mShader->SetUniformFloat3("material.specular",  mSpecular);
	mShader->SetUniformFloat( "material.metallic",  mMetallic);
	mShader->SetUniformFloat( "material.roughness", mRoughness);
	mShader->SetUniformFloat("material.iblStrength", mIblStrength);
	//mShader->SetUniformInt(   "useIBL",		   (int)mUseIBL);
}

void Ivy::Material::LoadTexturesAsync(std::vector<String> filepaths)
{
	std::for_each(filepaths.begin(), filepaths.end(), [](String &s) {
		mFutures.push_back(std::async(std::launch::async, LoadTextureAsync, s));
	});
	
	for(auto& data : mFutures)
	{
		Texture2DData td = data.get();
	
		Ptr<Texture2D> texture = CreatePtr<Texture2D>(td.width, td.height, td.data);
	
		if(texture)
		{
			Debug::CoreLog("Created Texture: {}", td.filepath);
			mLoadedTextures.insert({ td.filepath, texture });
		}
		td.Free();
	}


	/*
	tf::Executor executor;
	tf::Taskflow tf;

	for(auto& s : filepaths)
	{
		mFutures.push_back(executor.async(LoadTextureAsync, s));
		tf.emplace(LoadTextureAsync, s);
	}

	executor.run(tf);

	for(auto& data : mFutures)
	{
		Texture2DData td = data.get();

		Ptr<Texture2D> texture = CreatePtr<Texture2D>(td.width, td.height, td.data);

		if(texture)
		{
			Debug::CoreLog("Created Texture: {}", td.filepath);
			mLoadedTextures.insert({ td.filepath, texture });
		}
		td.Free();
	}
	*/
}


Ivy::Texture2DData Ivy::Material::LoadTextureAsync(String filepath)
{
	if(filepath.empty())
	{
		Debug::CoreWarning("Path to texture is empty. skipping texture loading! ({})", filepath);
		return{};
	}

	auto itr = mLoadedTextures.find(filepath);

	//Ptr<Texture2D> texture = nullptr;
	Texture2DData data = {};
	if(itr == mLoadedTextures.end())
	{
		//texture = CreatePtr<Texture2D>(filepath);
	
		data = Texture2D::LoadTextureData(filepath);
	}

	return data;


	//if(texture)
	//{
	//	Debug::CoreLog("Loaded texture file: {}", filepath);
	//	std::lock_guard<std::mutex> lock(gTextureMutex);
	//	mLoadedTextures.insert({ filepath, texture });
	//}
}
