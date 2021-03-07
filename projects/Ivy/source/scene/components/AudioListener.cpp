#include "ivypch.h"
#include "AudioListener.h"

Ivy::AudioListener::AudioListener(Entity* ent) : Ivy::Component::Component(ent)
{
	mTransform = mEntity->GetFirstComponentOfType<Transform>();
	UpdateValues();
}

void Ivy::AudioListener::OnUpdate(float deltaTime)
{
	UpdateValues();

	alListener3f(AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
	alListener3f(AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
	alListenerfv(AL_ORIENTATION, mListenerOrientation.data());
}

void Ivy::AudioListener::UpdateValues()
{
	mPosition = mTransform->getPosition();
	mListenerOrientation[0] = mTransform->getPosition().x;
	mListenerOrientation[1] = mTransform->getPosition().y;
	mListenerOrientation[2] = mTransform->getPosition().z;
	mListenerOrientation[3] = mTransform->getUp().x;
	mListenerOrientation[4] = mTransform->getUp().y;
	mListenerOrientation[5] = mTransform->getUp().z;
}
