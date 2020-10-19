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

Ivy::Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mID);
}

void Ivy::Texture2D::Load(String filepath)
{
	if (filepath.empty())
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
	mWidth = width;
	mHeight = height;

	GLenum internalFormat = 0, dataFormat = 0;
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

Ivy::TextureCube::TextureCube(Vector<String> filenames)
{
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mID);

	Load(filenames);
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
