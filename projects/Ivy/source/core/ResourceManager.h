#pragma once
#include "Types.h"
#include "environment/Log.h"


namespace Ivy
{
	class Resource
	{
	public:
		Resource()
			: mResourceID(mNextID++)
		{};

		virtual	   ~Resource() {}

		ResourceID GetResourceID() { return mResourceID; }
		bool	   IsLoaded()	   { return mIsLoaded; }

	protected:
		String	   mFilePath   = "";
		bool	   mIsLoaded   = false;
		ResourceID mResourceID = 0;

	private:
		inline static ResourceID mNextID = 0;
	};


	template<typename T>
	class ResourceManager
	{
	public:
		~ResourceManager() {}

		inline void AddResouce(Ptr<Resource> resource)
		{
			if (!resource)
			{
				Debug::CoreError("Resource is null!");
				return;
			}

			mResources[resource->GetResourceID()] = resource;
		}

		inline Ptr<T> GetResource(ResourceID id)
		{
			if (mResources[id]->mIsLoaded)
			{
				return static_cast<Ptr<T>>(mResources[id]);
			}
			else
			{
				return nullptr;
			}

		}

		inline bool IsLoaded(ResourceID id)
		{
			return mResources[id].IsLoaded();
		}

		static Ptr<ResourceManager> GetInstance()
		{
			if (!mInstance)
			{
				mInstance = CreatePtr<ResourceManager>();
			}

			return mInstance;
		}

	private:
		static Ptr<ResourceManager>				mInstance;
		UnorderedMap<ResourceID, Ptr<Resource>> mResources;
	};
}
