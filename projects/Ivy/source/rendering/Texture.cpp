#include "ivypch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Ivy::Texture2D::Texture2D(uint32_t width, uint32_t height)
	: mWidth(width)
	, mHeight(height)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &mID);
	glTextureStorage2D(mID, 1, mInternalFormat, mWidth, mHeight);

	glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(mID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mID, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	int width, height, channels;
	
	// Flips the the image vertically for correct UV mapping
	stbi_set_flip_vertically_on_load(1);

	stbi_uc* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

	mWidth = width;
	mHeight = height;

	if (!data)
	{
		Debug::CoreError("Couldn't load image file: {}", filepath);
	}

	if (channels == 4)
	{
		mInternalFormat = GL_RGBA8;
		mDataFormat = GL_RGBA;
	}
	else if (channels == 3)
	{
		mInternalFormat = GL_RGB8;
		mDataFormat = GL_RGB;
	}

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
	//glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mDataFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

void Ivy::Texture2D::SetData(void * data, uint32_t size)
{
}

void Ivy::Texture2D::Bind(uint32_t slot)
{
	glBindTextureUnit(slot, mID);
}
