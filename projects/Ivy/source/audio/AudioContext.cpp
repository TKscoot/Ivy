#include "ivypch.h"
#include "AudioContext.h"

Ivy::Ptr<Ivy::AudioContext> Ivy::AudioContext::mInstance = nullptr;

Ivy::AudioContext::AudioContext()
{
	OpenDefaultDevice();
	CreateContext();
}

Ivy::AudioContext::~AudioContext()
{
	alcCloseDevice(mCurrentDevice);
	alcDestroyContext(mAlcContext);

	mAlcContext	   = nullptr;
	mCurrentDevice = nullptr;
}

Ivy::Vector<Ivy::String> Ivy::AudioContext::GetAvailableOutputDevices()
{
	const ALCchar* devices = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
	
	const char* ptr = devices;

	Ivy::Vector<Ivy::String> devicesVec = {};


	// TODO Split devices string by \0

	return devicesVec;
}

void Ivy::AudioContext::OpenDefaultDevice()
{
	mCurrentDevice = alcOpenDevice(nullptr);

	if(!mCurrentDevice)
	{
		Debug::CoreError("Couldn't open default audio device!");
	}
}

void Ivy::AudioContext::OpenDevice(String name)
{
	mCurrentDevice = alcOpenDevice(name.c_str());

	if(!mCurrentDevice)
	{
		Debug::CoreError("Couldn't open {} audio device!", name);
	}
}

void Ivy::AudioContext::CreateContext()
{
	mAlcContext = alcCreateContext(mCurrentDevice, nullptr); // TODO: Eventuell hier bei flags: AL_ASYNC
	alcMakeContextCurrent(mAlcContext);
}
