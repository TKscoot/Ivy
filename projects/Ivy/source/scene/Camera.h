#pragma once
#include "Types.h"
#include "environment/Input.h"

namespace Ivy
{
	class Camera
	{
	public:

		Camera(Vec3 position)
			: mPosition(position)
		{
			UpdateCameraVectors();
		}

		void Update(float deltaTime);

		Mat4 GetViewMatrix();

		Vec3 GetPosition() { return mPosition; }

	private:
		void UpdateCameraVectors();

		// camera Attributes
		Vec3  mPosition = Vec3(0.0f);
		Vec3  mFront	= Vec3(0.0f, 0.0f, -1.0f);
		Vec3  mUp		= Vec3(0.0f, 1.0f, 0.0f);
		Vec3  mRight	= Vec3(1.0f, 0.0f, 0.0f);
		Vec3  mWorldUp	= Vec3(0.0f, 1.0f, 0.0f);

		Vec3 mVelocity = Vec3(0.0f);

		// euler Angles
		float mYaw   = -90.0f;
		float mPitch = 0.0f;

		// camera options
		float mMovementSpeed	= 5.0f;
		float mMouseSensitivity = 0.1f;
		float mZoom				= 45.0f;

		//movement
		bool mGoingForward = false;
		bool mGoingBack	   = false;
		bool mGoingLeft    = false;
		bool mGoingRight   = false;
		bool mGoingUp	   = false;
		bool mGoingDown    = false;
		bool mFastMove     = false;

		float mTopSpeed = 350.0f;

	};
}
