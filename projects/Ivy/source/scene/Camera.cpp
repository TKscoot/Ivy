#include "ivypch.h"
#include "Camera.h"

void Ivy::Camera::Update(float deltaTime)
{


	if(mHandleInput)
	{
		// Checks if movement keys are being pressed
		if(Input::IsKeyBeingPressed(W))		    mGoingForward = true;
		if(Input::IsKeyBeingPressed(S))	        mGoingBack	  = true;
		if(Input::IsKeyBeingPressed(A))	        mGoingLeft	  = true;
		if(Input::IsKeyBeingPressed(D))	        mGoingRight   = true;
		if(Input::IsKeyBeingPressed(Q))	        mGoingUp	  = true;
		if(Input::IsKeyBeingPressed(E))	        mGoingDown	  = true;
		if(Input::IsKeyBeingPressed(LeftShift)) mFastMove	  = true;

		// Checks if movement keys are being released
		if(Input::IsKeyUp(LeftShift)) mFastMove		= false;
		if(Input::IsKeyUp(E))	      mGoingDown	= false;
		if(Input::IsKeyUp(Q))	      mGoingUp		= false;
		if(Input::IsKeyUp(D))	      mGoingRight	= false;
		if(Input::IsKeyUp(A))	      mGoingLeft	= false;
		if(Input::IsKeyUp(S))	      mGoingBack	= false;
		if(Input::IsKeyUp(W))		  mGoingForward = false;

		// Movement calculation

		float velocity = mFastMove ? (mMovementSpeed + 20.0f) * deltaTime : mMovementSpeed * deltaTime;

		if(mGoingForward) mTransform->mPosition += mTransform->mFront * velocity;
		if(mGoingBack)    mTransform->mPosition -= mTransform->mFront * velocity;
		if(mGoingRight)   mTransform->mPosition += mTransform->mRight * velocity;
		if(mGoingLeft)    mTransform->mPosition -= mTransform->mRight * velocity;
		if(mGoingUp)      mTransform->mPosition += mTransform->mUp * velocity;
		if(mGoingDown)    mTransform->mPosition -= mTransform->mUp * velocity;

	}
	Vec2 mousePos = Input::GetMouseDelta();

	mousePos.x *= mMouseSensitivity;
	mousePos.y *= mMouseSensitivity;

	mYaw   += mousePos.x;
	mPitch += mousePos.y;

	if (mPitch > 89.0f)
		mPitch = 89.0f;
	if (mPitch < -89.0f)
		mPitch = -89.0f;
	if(mHandleInput)
	{
		UpdateCameraVectors();
	}
}

void Ivy::Camera::LookAt(Vec3 lookPos)
{
	mLookPos = lookPos;
}

Ivy::Mat4& Ivy::Camera::GetViewMatrix()
{
	if(mHandleInput)
	{
		mLookPos = mTransform->mPosition + mTransform->mFront;
	}
	mView = glm::lookAt(mTransform->mPosition, mLookPos, mTransform->mUp);
	return mView;
}

Ivy::Mat4& Ivy::Camera::GetProjectionMatrix(Vec2 currentWindowSize)
{
	mProjection = glm::perspective(glm::radians(mFOV), currentWindowSize.x / currentWindowSize.y, mNear, mFar);

	return mProjection;
}

void Ivy::Camera::UpdateCameraVectors()
{
	// calculate the new Front vector
	Vec3 front = Vec3(1.0f);
	front.x = glm::cos(glm::radians(mYaw)) * glm::cos(glm::radians(mPitch));
	front.y = glm::sin(glm::radians(mPitch));
	front.z = glm::sin(glm::radians(mYaw)) * glm::cos(glm::radians(mPitch));
	mTransform->mFront  = glm::normalize(front);

	// also re-calculate the Right and Up vector
	mTransform->mRight = glm::normalize(glm::cross(mTransform->mFront, mWorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	mTransform->mUp    = glm::normalize(glm::cross(mTransform->mRight, mTransform->mFront));
}
