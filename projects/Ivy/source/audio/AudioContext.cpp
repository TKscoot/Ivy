#include "ivypch.h"
#include "AudioContext.h"

Ivy::Ptr<Ivy::AudioContext> Ivy::AudioContext::mInstance = nullptr;

Ivy::AudioContext::AudioContext()
{
	CreateContext();
	OpenDefaultDevice();
}

Ivy::AudioContext::~AudioContext()
{
	mEngine->drop();
}

Ivy::Vector<Ivy::String> Ivy::AudioContext::GetAvailableOutputDevices()
{
	ISoundDeviceList* dl = createSoundDeviceList();

	Vector<String> devicesVec;

	for(int i = 0; i < dl->getDeviceCount(); i++)
	{
		String desc = dl->getDeviceDescription(i);
		devicesVec.push_back(desc);
	}

	dl->drop();
	
	return devicesVec;
}

void Ivy::AudioContext::OpenDefaultDevice()
{

}

void Ivy::AudioContext::OpenDevice(String name)
{

}

void Ivy::AudioContext::CreateContext()
{
	mEngine = createIrrKlangDevice();
}
