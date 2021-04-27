#include "ivypch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Ivy::Texture2D::Texture2D(uint32_t width, uint32_t height)
	: mWidth(width)
	, mHeight(height)
{
	mInternalFormat = GL_RGBA8;
	mDataFormat		= GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &mID);
	glTextureStorage2D(mID, 1, mInternalFormat, mWidth, mHeight);

	glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(mID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Ivy::Texture2D::Texture2D(uint32_t width, uint32_t height, void * data)
	: mWidth(width)
	, mHeight(height)
{
	mInternalFormat = GL_RGBA8;
	mDataFormat     = GL_RGBA;

	if (!mID)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &mID);
	}
	glTextureStorage2D(mID, 1, mInternalFormat, mWidth, mHeight);

	glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(mID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(mID, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);
}

Ivy::Texture2D::Texture2D(String filepath)
	: mWidth(0)
	, mHeight(0)
{
	Load(filepath);
}

Ivy::Texture2D::Texture2D(String filepath, GLenum internalFormat, GLenum dataFormat)
	: mInternalFormat(internalFormat)
	, mDataFormat(dataFormat)
{
	Load(filepath, internalFormat, dataFormat);
}

Ivy::Texture2D::~Texture2D()
{
	Destroy();
}

void Ivy::Texture2D::Load(String filepath)
{
	mFilepath = filepath;
	if (filepath.empty())
	{
		Debug::CoreError("Filepath is empty!");
		return;
	}

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		data = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		
	}
	if(!data)
	{
		Debug::CoreError("Couldn't load texture with path: {}", filepath);
	}

	mWidth = width;
	mHeight = height;

	GLenum internalFormat = 0, dataFormat = 0;
	channels = 4;
	if (channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}
	else if(channels == 2)
	{
		internalFormat = GL_RG8;
		dataFormat = GL_RG;
	}
	else if(channels == 1)
	{
		internalFormat = GL_R8;
		dataFormat = GL_R;
	}

	mInternalFormat = internalFormat;
	mDataFormat = dataFormat;


	glCreateTextures(GL_TEXTURE_2D, 1, &mID);
	glTextureStorage2D(mID, 1, internalFormat, mWidth, mHeight);

	glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(mID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(mID, 0, 0, 0, mWidth, mHeight, dataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	stbi_set_flip_vertically_on_load(0);

}

void Ivy::Texture2D::Load(String filepath, GLenum internalFormat, GLenum dataFormat)
{
	if(filepath.empty())
	{
		Debug::CoreError("Filepath is empty!");
		return;
	}

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
	}
	if(!data)
	{
		Debug::CoreError("Couldn't load texture with path: {}", filepath);
	}

	mWidth  = width;
	mHeight = height;

	mInternalFormat = internalFormat;
	mDataFormat		= dataFormat;


	glCreateTextures(GL_TEXTURE_2D, 1, &mID);
	glTextureStorage2D(mID, 1, internalFormat, mWidth, mHeight);

	glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(mID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(mID, 0, 0, 0, mWidth, mHeight, dataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

void Ivy::Texture2D::SetData(void * data, uint32_t size)
{
	glTextureStorage2D(mID, 1, mInternalFormat, mWidth, mHeight);

	glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(mID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(mID, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);
}

void Ivy::Texture2D::Bind(uint32_t slot)
{
	if(	glIsTexture(mID))
	{
		glBindTextureUnit(slot, mID);
	}
}

Ivy::Texture2DData Ivy::Texture2D::LoadTextureData(String file)
{
	Texture2DData texData = {};


	if(file.empty())
	{
		Debug::CoreError("Filepath is empty!");
		return {};
	}
	texData.filepath = file;

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		data = stbi_load(file.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	}
	if(!data)
	{
		Debug::CoreError("Couldn't load texture with path: {}", file);
	}

	texData.data = data;
	texData.width = width;
	texData.height = height;

	GLenum internalFormat = 0, dataFormat = 0;
	channels = 4;
	if(channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat     = GL_RGBA;
	}
	else if(channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}
	else if(channels == 2)
	{
		internalFormat = GL_RG8;
		dataFormat = GL_RG;
	}
	else if(channels == 1)
	{
		internalFormat = GL_R8;
		dataFormat = GL_R;
	}

	texData.internalFormat = internalFormat;
	texData.dataFormat = dataFormat;

	Debug::CoreLog("TEXTURE ASYNC: Loaded texture file: {}", texData.filepath);
	return texData;
}

void Ivy::Texture2D::Destroy()
{
	glDeleteTextures(1, &mID);
}

Ivy::TextureCube::TextureCube(Vector<String> filenames)
{
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mID);

	Load(filenames);
}

Ivy::TextureCube::~TextureCube()
{
	Destroy();
}

uint32_t CalculateMipMapCount(uint32_t width, uint32_t height)
{
	uint32_t levels = 1;
	while((width | height) >> levels)
		levels++;

	return levels;
}

Ivy::TextureCube::TextureCube(GLenum format, uint32_t width, uint32_t height)
{
	mWidth = width;
	mHeight = height;

	uint32_t levels = CalculateMipMapCount(width, height);

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &mID);
	glTextureStorage2D( mID, levels, format, width, height);
	glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Ivy::TextureCube::TextureCube(String right, String left, String top, String bottom, String back, String front)
{


	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
	
	Load(right,
		left,
		top,
		bottom,
		back,
		front);
}

void Ivy::TextureCube::Load(Vector<String> filenames)
{
	int width, height, nrChannels;
	for (unsigned int i = 0; i < filenames.size(); i++)
	{
		unsigned char *data = stbi_load(filenames[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			Debug::CoreError("Cubemap texture failed to load at path: {}", filenames[i]);
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	mWidth = width;
	mHeight = height;
}

void Ivy::TextureCube::Destroy()
{
	glDeleteTextures(1, &mID);
	mID = 0;
}

void Ivy::TextureCube::Load(String right, String left, String top, String bottom, String back, String front)
{
	Vector<String> filenames;
	filenames.push_back(right);
	filenames.push_back(left);
	filenames.push_back(top);
	filenames.push_back(bottom);
	filenames.push_back(back);
	filenames.push_back(front);

	Load(filenames);
}

void Ivy::TextureCube::Bind(uint32_t slot)
{
	if (glIsTexture(mID))
	{
		glBindTextureUnit(slot, mID);
	}
}

Ivy::TextureHDRI::TextureHDRI(String filename)
{
	Load(filename);
}

Ivy::TextureHDRI::~TextureHDRI()
{
	Destroy();
}

void Ivy::TextureHDRI::Destroy()
{
	glDeleteTextures(1, &mID);
	mID = 0;
}

void Ivy::TextureHDRI::Load(String filename)
{
	//stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 0);
	if(data)
	{
		glGenTextures(1, &mID);
		glBindTexture(GL_TEXTURE_2D, mID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		Debug::CoreError("Failed to load HDR image: {}", filename);
	}

}

void Ivy::TextureHDRI::Bind(uint32_t slot)
{
	glBindTextureUnit(slot, mID);
}

 void Ivy::TextureHDRI::ConvertToCubemap(Ptr<TextureCube> outputTexCube)
{
	Ptr<Shader> equirectangularConversionShader = CreatePtr<Shader>("shaders/EquirectangularToCubeMap.comp");

	equirectangularConversionShader->Bind();
	Bind();
	glBindImageTexture(0, outputTexCube->GetID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(outputTexCube->GetWidth() / 32, outputTexCube->GetHeight() / 32, 6);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glGenerateTextureMipmap(outputTexCube->GetID());

	Destroy();
}
