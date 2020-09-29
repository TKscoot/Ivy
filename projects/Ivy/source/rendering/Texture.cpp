#include "ivypch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Ivy::AssetHolder* Ivy::AssetHolder::mInstance = nullptr;

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
	glGenerateMipmap(GL_TEXTURE_2D);
}

Ivy::Texture2D::Texture2D(String filepath)
	: mWidth(0)
	, mHeight(0)
{
	Load(filepath);
}

Ivy::Ptr<Ivy::Texture2D> Ivy::Texture2D::Create(uint32_t width, uint32_t height)
{
	return Ptr<Texture2D>(new Texture2D(width, height));
}

Ivy::Ptr<Ivy::Texture2D> Ivy::Texture2D::Create(uint32_t width, uint32_t height, void * data)
{
	return Ptr<Texture2D>(new Texture2D(width, height, data));
}

Ivy::Ptr<Ivy::Texture2D> Ivy::Texture2D::Create(String filepath)
{
	//auto itr = mLoadedTextures.find(filepath);
	//
	//if (itr != mLoadedTextures.end())
	//{
	//	std::shared_ptr<Texture2D> sharedTex(itr->second);
	//	return sharedTex; // TODO: ERROR texture is immutable! Use Resource manager
	//}
	//else
	//{
	//	Texture2D* tex = new Texture2D(filepath);
	//	tex->Load(filepath);
	//	Ptr<Texture2D> sharedTex(tex);
	//	return sharedTex;
	//}
	return AssetHolder::GetInstance()->getTexture(filepath);
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
	//auto itr = mLoadedTextures.find(filepath);

	//if (itr != mLoadedTextures.end())
	//{
	//	//this. = *itr->second; // TODO: Eventuell buggy copying
	//	mID				= itr->second->mID;
	//	mInternalFormat = itr->second->mInternalFormat;
	//	mDataFormat		= itr->second->mDataFormat;
	//	mWidth			= itr->second->mWidth;
	//	mHeight			= itr->second->mHeight;
	//	mIsLoaded		= true;
	//}
	//else
	//{
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
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		mIsLoaded = true;
		//mLoadedTextures[filepath] = this;
		//ResourceManager<Texture2D>::GetInstance()->AddResouce(CreatePtr<Texture2D>(this));
	//}
}

void Ivy::Texture2D::SetData(void * data, uint32_t size)
{
	glTextureStorage2D(mID, 1, mInternalFormat, mWidth, mHeight);

	glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(mID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(mID, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Ivy::Texture2D::Bind(uint32_t slot)
{
	if(	glIsTexture(mID))
	{
		glBindTextureUnit(slot, mID);
	}
}
