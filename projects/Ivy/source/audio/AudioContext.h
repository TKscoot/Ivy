#pragma once
#include "AL/al.h"
#include "AL/alc.h"

#include "OpenALHelpers.h"

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

	private:
		// Methods
		void OpenDefaultDevice();
		void OpenDevice(String name);
		void CreateContext();


		// Variables
		static Ptr<AudioContext> mInstance;

		// OpenAL Handles
		ALCcontext* mAlcContext    = nullptr;
		ALCdevice*  mCurrentDevice = nullptr;

	};
}