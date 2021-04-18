#pragma once
#include <irrKlang.h>

#include "Types.h"

using namespace irrklang;

namespace Ivy
{
	class AudioContext
	{
	public:
		AudioContext();
		~AudioContext();

		Vector<String> GetAvailableOutputDevices();

		static Ptr<AudioContext> GetInstance()
		{
			if(!mInstance)
			{
				mInstance = CreatePtr<AudioContext>();
			}

			return mInstance;
		}

		ISoundEngine* GetEngine() { return mEngine; }


	private:
		// Methods
		void OpenDefaultDevice();
		void OpenDevice(String name);
		void CreateContext();


		// Variables
		static Ptr<AudioContext> mInstance;

		ISoundEngine* mEngine = nullptr;
	};
}