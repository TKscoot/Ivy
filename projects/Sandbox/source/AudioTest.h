#pragma once
#include <Ivy.h>
#include <sstream>
using namespace Ivy;

class AudioTest : public Entity
{
public:
	AudioTest();

	void OnCreate() final;
	void OnUpdate(float deltaTime) final;
	void OnDestroy() final;

private:
	Ptr<AudioClip> mAudioClip = nullptr;

	Vector<String> mSoundFiles;
};