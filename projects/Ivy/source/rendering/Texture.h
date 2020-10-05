#pragma once
#include <glad/glad.h>
#include <map>
#include "Types.h"
#include "environment/Log.h"
#include "core/ResourceManager.h"

namespace Ivy
{
	class Texture2D
	{
	public:

		Texture2D(uint32_t width, uint32_t height);
		Texture2D(uint32_t width, uint32_t height, void* data);
		Texture2D(String filepath);		

		~Texture2D();

		void Load(String filepath);

		void SetData(void* data, uint32_t size);
		void Bind(uint32_t slot = 0);

		uint32_t GetWidth()  const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }

		GLuint GetRendererID() const { return mID; }

		// TODO: void SetPixel(Vec2i coord, Vec4 colorValues); 
	private:
		GLuint	 mID			 = 0;
		GLenum	 mInternalFormat;
		GLenum	 mDataFormat;
		uint32_t mWidth          = 0;
		uint32_t mHeight         = 0;

		bool mIsLoaded = true;

		//inline static UnorderedMap<String, Texture2D*> mLoadedTextures = {};

	};

}