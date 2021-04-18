#include "ivypch.h"
#include "FModContext.h"

Ivy::Ptr<Ivy::FModContext> Ivy::FModContext::mInstance = nullptr;

Ivy::FModContext::FModContext()
{
	FMOD_RESULT result;

	// Create the main system object.
	result = FMOD::System_Create(&mSystem);
	if(!succeededOrWarn("FMOD: Failed to create system object", result))
		return;

	// Initialize FMOD.
	result = mSystem->init(512, FMOD_INIT_NORMAL, nullptr);
	if(!succeededOrWarn("FMOD: Failed to initialise system object", result))
		return;

	// Create the channel group.
	result = mSystem->createChannelGroup("Default", &mChannelGroup);
	if(!succeededOrWarn("FMOD: Failed to create in-game sound effects channel group", result))
		return;

	// Create the sound.
	FMOD::Sound *sound = nullptr;
	mSystem->createSound("assets/sounds/Polyphia - Nasty.flac", FMOD_CREATESTREAM, nullptr, &sound);
	if(!succeededOrWarn("FMOD: Failed to create sound", result))
		return;

	// Play the sound.
	FMOD::Channel *channel = nullptr;
	result = mSystem->playSound(sound, mChannelGroup, true, &channel);
	if(!succeededOrWarn("FMOD: Failed to play sound", result))
		return;
	FMOD_VECTOR pos = { -10.0f, 0.0f, 0.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	channel->set3DAttributes(&pos, &vel);


	// Assign the channel to the group.
	//result = channel->setChannelGroup(mChannelGroup);
	//if(!succeededOrWarn("FMOD: Failed to set channel group on", result))
	//	return;

}

Ivy::FModContext::~FModContext()
{
	mChannelGroup->release();
	mSystem->release();

}

void Ivy::FModContext::Update()
{
	mSystem->update();
}
