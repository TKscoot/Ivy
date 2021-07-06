#include "ivypch.h"
#include "AudioClip3D.h"

Ivy::AudioClip3D::AudioClip3D(Ptr<Entity> entity, Ptr<Transform> transform, String filename, bool playOnStart)
	: AudioClip::AudioClip(entity)
{
	mTransform = transform;
	mFilePath = filename;
	mPlayOnStart = playOnStart;

	mContext = AudioContext::GetInstance();

	mSound = mContext->GetEngine()->play3D(
			 mFilePath.c_str(), 
			 mContext->Vec3ToVec3df(mTransform->getPosition()),
			 false, true, true);
	mSound->setMinDistance(5.0f);

	mSound->setSoundStopEventReceiver(this);
}

void Ivy::AudioClip3D::AddSoundClip(String filename)
{
	mFilePath = filename;
	Stop();
	mSound->drop();
	mSound = nullptr;	// TODO: Eventuell buggy
	mSound = mContext->GetEngine()->play3D(
			 mFilePath.c_str(),
			 mContext->Vec3ToVec3df(mTransform->getPosition()),
			 false, true, true);

	mSound->setSoundStopEventReceiver(this);
}

void Ivy::AudioClip3D::OnSoundStopped(ISound* sound, E_STOP_EVENT_CAUSE reason, void* userData)
{
	float volume		  = GetVolume();
	float pan			  = GetPan();
	float speed			  = GetPlaybackSpeed();
	int   duration		  = GetDuration();
	int   currentPosition = GetPlaybackPosition();
	bool  loop			  = IsLooping();

	mSound->drop();
	mSound = nullptr;
	mSound = mContext->GetEngine()->play3D(
			 mFilePath.c_str(),
			 mContext->Vec3ToVec3df(mTransform->getPosition()),
			 false, true, true);

	mSound->setSoundStopEventReceiver(this);

	SetVolume(volume);
	SetPan(pan);
	SetPlaybackSpeed(speed);
	ShouldLoop(loop);
}
