#include "ivypch.h"
#include "AudioStreamSource.h"

Ivy::AudioStreamSource::AudioStreamSource(String filename)
{
	auto system = FModContext::GetInstance()->GetSystem();
	system->createSound(
		filename.c_str(),
		FMOD_CREATESTREAM, 
		nullptr, 
		&mSound);

	system->playSound(mSound, FModContext::GetInstance()->GetChannelGroup(), true, &mChannel);
	mChannel->setChannelGroup(FModContext::GetInstance()->GetChannelGroup());
}

void Ivy::AudioStreamSource::Play()
{
	mChannel->stop();
	mChannel->setPaused(false);
}

void Ivy::AudioStreamSource::Pause()
{
	mChannel->setPaused(true);
}

void Ivy::AudioStreamSource::Stop()
{
	mChannel->stop();
}

void Ivy::AudioStreamSource::Resume()
{
	mChannel->setPaused(false);
}

void Ivy::AudioStreamSource::AddSoundClip(String filename)
{
	auto system = FModContext::GetInstance()->GetSystem();
	system->createSound(
		filename.c_str(),
		FMOD_CREATESTREAM,
		nullptr,
		&mSound);
	system->playSound(mSound, FModContext::GetInstance()->GetChannelGroup(), true, &mChannel);
}
