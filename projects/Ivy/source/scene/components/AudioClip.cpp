#include "ivypch.h"
#include "AudioClip.h"

Ivy::AudioClip::AudioClip(Entity * ent, String filename, bool playOnStart) : Ivy::Component::Component(ent)
{
	AudioContext::GetInstance()->GetAvailableOutputDevices();
	mPlayOnStart = playOnStart;
	mAudioStream = CreatePtr<AudioStreamSource>(filename);
}

void Ivy::AudioClip::AddSoundClip(String filename)
{
	mAudioStream->AddSoundClip(filename);
}

void Ivy::AudioClip::Play()
{
	mAudioStream->Play();
}

void Ivy::AudioClip::Pause()
{
	mAudioStream->Pause();
}

void Ivy::AudioClip::Resume()
{
	mAudioStream->Resume();
}

void Ivy::AudioClip::Stop()
{
	mAudioStream->Stop();
}

void Ivy::AudioClip::ShouldLoop(bool loop)
{
	mAudioStream->ShouldLoop(loop);
}

void Ivy::AudioClip::OnStart()
{
	if(mPlayOnStart)
	{
		Play();
	}
}

void Ivy::AudioClip::OnUpdate(float deltatime)
{
	mAudioStream->Update();
}
