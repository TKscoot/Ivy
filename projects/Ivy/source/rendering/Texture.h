#pragma once
#include <glad/glad.h>
#include <map>
#include "Types.h"
#include "environment/Log.h"
#include "core/ResourceManager.h"

namespace Ivy
{
	class AssetHolder;

	class Texture2D : public Resource
	{
	public:

		static Ptr<Texture2D> Create(uint32_t width, uint32_t height);
		static Ptr<Texture2D> Create(uint32_t width, uint32_t height, void* data);
		static Ptr<Texture2D> Create(String filepath);

		~Texture2D();

		void Load(String filepath);

		void SetData(void* data, uint32_t size);
		void Bind(uint32_t slot = 0);

		uint32_t GetWidth()  const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }

		// TODO: void SetPixel(Vec2i coord, Vec4 colorValues); 
		Texture2D(String filepath);		
	private:
		Texture2D(uint32_t width, uint32_t height);
		Texture2D(uint32_t width, uint32_t height, void* data);
		GLuint	 mID			 = 0;
		GLenum	 mInternalFormat;
		GLenum	 mDataFormat;
		uint32_t mWidth          = 0;
		uint32_t mHeight         = 0;

		bool mIsLoaded = true;

		//inline static UnorderedMap<String, Texture2D*> mLoadedTextures = {};
		Ptr<AssetHolder> mAssetHolder;

	};

	class AssetHolder
	{
	public:
		const Ptr<Texture2D> &getTexture(std::string file)
		{
			auto a = mTextures.find(file);

			if (a != mTextures.end()) // Exists already
				return a->second; // Just return it

			// Otherwise load the texture and save it for later
			Ptr<Texture2D> tex = CreatePtr<Texture2D>(file);
			mTextures[file] = tex; // Implicit creation


			return tex; // Return the texture
		}

		static AssetHolder* GetInstance()
		{
			if (!mInstance)
			{
				mInstance = new AssetHolder();
			}

			return mInstance;
		}

	private:
		static AssetHolder* mInstance;
		std::map<std::string, Ptr<Texture2D>> mTextures;
	};

}