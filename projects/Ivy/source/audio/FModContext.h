#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <Types.h>
#include <environment/Log.h>

namespace Ivy
{

	
	//FMOD_RESULT F_CALLBACK channelGroupCallback(FMOD_CHANNELCONTROL *channelControl,
	//	FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
	//	void *commandData1, void *commandData2)
	//{
	//	Debug::CoreLog("Callback called for {}", controlType);
	//	return FMOD_OK;
	//}


	class FModContext
	{
	public:
		FModContext();
		~FModContext();

		//Vector<String> GetAvailableOutputDevices();

		void Update();

		bool succeededOrWarn(const std::string &message, FMOD_RESULT result)
		{
			if(result != FMOD_OK)
			{
				Debug::CoreWarning("{}{}{} {}", message, ": ", result, FMOD_ErrorString(result));
				return false;
			}
			return true;
		}

		static Ptr<FModContext> GetInstance()
		{
			if(!mInstance)
			{
				mInstance = CreatePtr<FModContext>();
			}

			return mInstance;
		}

		FMOD::System* GetSystem() { return mSystem; }
		FMOD::ChannelGroup* GetChannelGroup() { return mChannelGroup; }

	private:

		// Variables
		static Ptr<FModContext> mInstance;

		FMOD::System* mSystem = nullptr;
		FMOD::ChannelGroup *mChannelGroup = nullptr;

	};
}