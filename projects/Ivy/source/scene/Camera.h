#pragma once
#include "Types.h"
#include "environment/Input.h"
#include "components/Transform.h"

namespace Ivy
{
	/*!
	 * The camera to view the 3D scene. Default camera behaviour is a flythrough camera.
	 * 
	 */
	class Camera
	{
	public:
		/*!
		 * Constructor
		 * 
		 * \param position The initial position of the camera.
		 */
		Camera(Vec3 position)
		{
			mTransform = CreatePtr<Transform>(nullptr);
			mTransform->mPosition = position;
			mView = glm::lookAt(mTransform->mPosition, mTransform->mPosition + mTransform->mFront, mTransform->mUp);
			UpdateCameraVectors();
		}

		/*!
		 * Updates the camera
		 * 
		 * Internal! No need to call this!
		 * 
		 * \param deltaTime
		 */
		void Update(float deltaTime);

		/*!
		 * Sets the point where the camera is looking at
		 * 
		 * \param lookPos Position vector to look at
		 */
		void LookAt(Vec3 lookPos);

		Vec3 GetLookAt() { return mLookPos; }

		/*!
		 * 
		 * \return Returns the view matrix
		 */
		Mat4& GetViewMatrix();

		/*!
		 * Calculates the projection matrix
		 * 
		 * \param currentWindowSize The currently used window size in pixels
		 * \return Returns the projection matrix
		 */
		Mat4& GetProjectionMatrix(Vec2 currentWindowSize);

		/*!
		 * Gets the world position of the camera
		 * 
		 * \return Returns world position
		 */
		Vec3 GetPosition() { return mTransform->mPosition; }

		/*!
		 * Sets the world position of the camera
		 * 
		 * \param position Desired world position
		 */
		void SetPosition(Vec3 position) { mTransform->mPosition = position; }

		/*!
		 * Sets the X rotation
		 * 
		 * \param rotation Rotation in degrees
		 */
		void SetRotation(float rotation) 
		{
			mTransform->mFront.x = glm::cos(glm::radians(rotation)) * glm::cos(glm::radians(rotation));
			mTransform->mFront = glm::normalize(mTransform->mFront);
		}

		void SetFront(Vec3 front) {
			mTransform->mFront = front;
		}

		/*!
		 * Gets the front vector
		 * 
		 */
		Vec3 GetFront() { return mTransform->mFront; }

		/*!
		 * Checks and handles input events
		 * 
		 * \param shouldHandle Wether the input should be checked
		 */
		void HandleInput(bool shouldHandle) { mHandleInput = shouldHandle; }

		/*!
		 * Distance to the near plane
		 * 
		 * \return Near plane of the camera
		 */
		float GetNearPlane() { return mNear; }

		/*!
		 * Distance to the far plane
		 * 
		 * \return Far plane of the camera
		 */
		float GetFarPlane() { return mFar; }

		/*!
		 * Sets the Field of View
		 * 
		 * \param fov Field of View in degrees
		 */
		void SetFOV(float fov) { mFOV = fov; }

		float GetFOV() { return mFOV; }

		/*!
		 * Resets the camera to default values
		 * 
		 */
		void Reset()
		{
			mTransform->mPosition = Vec3(0.0f);
			mTransform->mFront	  = Vec3(0.0f, 0.0f, -1.0f);
			mTransform->mUp		  = Vec3(0.0f, 1.0f, 0.0f);
			mTransform->mRight    = Vec3(1.0f, 0.0f, 0.0f);

			mWorldUp  = Vec3(0.0f, 1.0f, 0.0f);

			mVelocity = Vec3(0.0f);

			mLookPos  = Vec3(0.0f);

			mProjection = Mat4(0.0f);
			mView		= Mat4(0.0f);
			mFOV		= 60.0f;
			mNear		= 0.1f;
			mFar		= 512.0f;
		}

		Ptr<Transform> GetTransform() { return mTransform; }

	private:
		void UpdateCameraVectors();

		Ptr<Transform> mTransform = nullptr;

		// camera Attributes
		//Vec3  mPosition = Vec3(0.0f);
		//Vec3  mFront	= Vec3(0.0f, 0.0f, 1.0f);
		//Vec3  mUp		= Vec3(0.0f, 1.0f, 0.0f);
		//Vec3  mRight	= Vec3(1.0f, 0.0f, 0.0f);
		Vec3  mWorldUp	= Vec3(0.0f, 1.0f, 0.0f);


		Vec3  mVelocity = Vec3(0.0f);

		Vec3  mLookPos  = Vec3(0.0f);

		Mat4  mProjection = Mat4(0.0f);
		Mat4  mView		  = Mat4(0.0f);
		float mFOV		  = 60.0f;
		float mNear		  = 0.1f;
		float mFar		  = 512.0f;

		// euler Angles
		float mYaw   = -90.0f;
		float mPitch = 0.0f;

		// camera options
		float mMovementSpeed	= 5.0f;
		float mMouseSensitivity = 0.1f;

		//movement
		bool mGoingForward = false;
		bool mGoingBack	   = false;
		bool mGoingLeft    = false;
		bool mGoingRight   = false;
		bool mGoingUp	   = false;
		bool mGoingDown    = false;
		bool mFastMove     = false;

		float mTopSpeed = 350.0f;

		bool mHandleInput = true;

	};
}
