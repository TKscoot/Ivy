#include "ivypch.h"
#include "AudioClip.h"

Ivy::AudioClip::AudioClip(Ptr<Entity> entity, String filename, bool playOnStart) : Ivy::Component::Component(entity)
{
	mFilePath = filename;
	mPlayOnStart = playOnStart;

	mContext = AudioContext::GetInstance();

	mSound = mContext->GetEngine()->play2D(mFilePath.c_str(), false, true, true);
	mSound->setSoundStopEventReceiver(this);
}

void Ivy::AudioClip::AddSoundClip(String filename)
{
	mFilePath = filename;
	Stop();
	mSound->drop();
	mSound = nullptr;	// TODO: Eventuell buggy
	mSound = mContext->GetEngine()->play2D(mFilePath.c_str(), false, true, true);
	mSound->setSoundStopEventReceiver(this);
}

void Ivy::AudioClip::Play()
{
	Stop();
	mSound->setIsPaused(false);
	mSound->setPlayPosition(mPlayPositionAfterPause);
}

void Ivy::AudioClip::Pause()
{
	mPlayPositionAfterPause = mSound->getPlayPosition();
	mSound->setIsPaused(true);
}

void Ivy::AudioClip::Resume()
{
	if(mSound->getIsPaused())
	{
		mSound->setIsPaused(false);
		mSound->setPlayPosition(mPlayPositionAfterPause);
	}
}

void Ivy::AudioClip::Stop()
{
	mSound->setIsPaused(true);
	mPlayPositionAfterPause = 0;
}

void Ivy::AudioClip::OnSoundStopped(ISound * sound, E_STOP_EVENT_CAUSE reason, void * userData)
{
	float volume		  = GetVolume();
	float pan			  = GetPan();
	float speed			  = GetPlaybackSpeed();
	int   duration		  = GetDuration();
	int   currentPosition = GetPlaybackPosition();
	bool  loop			  = IsLooping();

	mSound->drop();
	mSound = nullptr;
	mSound = mContext->GetEngine()->play2D(mFilePath.c_str(), false, true, true);
	mSound->setSoundStopEventReceiver(this);

	SetVolume(volume);
	SetPan(pan);
	SetPlaybackSpeed(speed);
	ShouldLoop(loop);


}

float Ivy::AudioClip::GetVolume()
{
	return (mSound != nullptr) ? mSound->getVolume() : -1.0f;
}

void Ivy::AudioClip::SetVolume(float volume)
{
	mSound->setVolume(volume);
}

float Ivy::AudioClip::GetPan()
{
	return (mSound != nullptr) ? mSound->getPan() : -2.0f;
}

void Ivy::AudioClip::SetPan(float pan)
{
	mSound->setPan(pan);
}

bool Ivy::AudioClip::IsFinished()
{
	return (mSound != nullptr) ? mSound->isFinished() : true;
}

int Ivy::AudioClip::GetDuration()
{
	return (mSound != nullptr) ? mSound->getPlayLength() : -1.0f;
}

int Ivy::AudioClip::GetPlaybackPosition()
{
	return (mSound != nullptr) ? mSound->getPlayPosition() : -1.0f;
}

void Ivy::AudioClip::SetPlaybackPosition(int position)
{
	mSound->setPlayPosition(position);
}

float Ivy::AudioClip::GetPlaybackSpeed()
{
	return (mSound != nullptr) ? mSound->getPlaybackSpeed() : -1.0f;
}

void Ivy::AudioClip::SetPlaybackSpeed(float speed)
{
	mSound->setPlaybackSpeed(speed);
}

void Ivy::AudioClip::ShouldLoop(bool loop)
{
	mSound->setIsLooped(loop);
}

bool Ivy::AudioClip::IsLooping()
{
	return (mSound != nullptr) ? mSound->isLooped() : false;
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
	
}
