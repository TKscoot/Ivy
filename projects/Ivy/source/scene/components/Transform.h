#pragma once
#include "Types.h"
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/norm.hpp>

#include "scene/components/Component.h"

namespace Ivy
{
	class Transform : public Component
	{
	public:

		friend class Camera;


		Transform(Ptr<Entity> entity, Transform* parent = nullptr)
			: Component::Component(entity)
			, mScale({ 1.0f, 1.0f, 1.0f })
			, mRotation(Vec3())
			, mPosition({ 0.0f, 0.0f, 0.0f })
			, mFront({ 0.0f, 0.0f, 1.0f })
			, mRight({ 1.0f, 0.0f, 0.0f })
			, mUp({ 0.0f, 1.0f, 0.0f })
			, mComposed(Mat4(1.0f))
			, mParent(parent)
		{
		}

		void SetParent(Transform* parent) {
			mParent = parent;
		}

		inline const Vec3&  getDirection() 
		{
			Vec3 radRot = glm::radians(mRotation);
			mFront.x = glm::sin(radRot.y) * glm::cos(radRot.x);
			mFront.y = glm::sin(-radRot.x) * glm::cos(radRot.x);
			mFront.z = glm::cos(radRot.x) * glm::cos(radRot.y);
			return mFront; 
		};

		inline const Vec3&  getUp()        const { return mUp; };
		inline const Vec3&  getRight()     const { return mRight; };
		inline const Vec3&  getRotation()  const { return mRotation; }
		inline const Vec3&  getPosition()  const { return mPosition; }

		inline void setPositionX(const float& x) { mPosition.x = x; invalidate(); }
		inline void setPositionY(const float& y) { mPosition.y = y; invalidate(); }
		inline void setPositionZ(const float& z) { mPosition.z = z; invalidate(); }
		inline void setPosition(
			const float& x,
			const float& y,
			const float& z)
		{
			setPositionX(x);
			setPositionY(y);
			setPositionZ(z);
			invalidate();
		}
		inline void setPosition(const Vec3& vec) { mPosition = vec; invalidate(); }

		inline void setRotationX(const float& x) { mRotation.x = x; invalidate(); }
		inline void setRotationY(const float& y) { mRotation.y = y; invalidate(); }
		inline void setRotationZ(const float& z) { mRotation.z = z; invalidate(); }
		inline void setRotation(
			const float& x,
			const float& y,
			const float& z)
		{
			setRotationX(x);
			setRotationY(y);
			setRotationZ(z);
			invalidate();
		}
		inline void setRotation(const Vec3& vec) { mRotation = Vec4(vec, 1.0f); invalidate(); }
		inline void rotate(const Vec3& delta)
		{
			mRotation += delta; invalidate();
		}

		inline void setScaleX(const float& factor) { mScale.x = factor; invalidate(); }
		inline void setScaleY(const float& factor) { mScale.y = factor; invalidate(); }
		inline void setScaleZ(const float& factor) { mScale.z = factor; invalidate(); }
		inline void setScale(
			const float& x,
			const float& y,
			const float& z)
		{
			setScaleX(x);
			setScaleY(y);
			setScaleZ(z);
			invalidate();
		}
		inline void setScale(const Vec3& vec) { mScale = Vec4(vec, 1.0f); invalidate(); }
		
		inline const Vec3& getScale() const { return mScale; }

		inline const Mat4& getComposed() { invalidate(); return mComposed; }

		inline void invalidate()
		{
			//m_rotationMat = glm::eulerAngleXYZ(m_rotation.x, m_rotation.y, m_rotation.z);
			Mat4 rot = Mat4(1.0);
			Mat4 trans = Mat4(1.0);
			Mat4 scale = Mat4(1.0);
			rot = glm::rotate(rot, glm::radians(mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			rot = glm::rotate(rot, glm::radians(mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			rot = glm::rotate(rot, glm::radians(mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			trans = glm::translate(trans, mPosition);
			scale = glm::scale(scale, mScale);

			//mComposed = scale * rot * trans;
			if (mParent) 
			{
				mComposed = trans * scale * rot;
				mComposed = mParent->getComposed() * mComposed;
			}
			else 
			{
				mComposed = trans * scale * rot;
			}

		}



	private:
		Transform* mParent = nullptr;

		Vec3  mPosition    = Vec3(0.0f, 1.0f, 0.0f);
		Vec3  mRotation	   = Vec3(0.0f, 1.0f, 0.0f);
		Vec3  mScale	   = Vec3(1.0f, 1.0f, 1.0f);
		Mat4  mRotationMat = Mat4();

		Vec3  mFront;
		Vec3  mRight;
		Vec3  mUp;

		Mat4 mComposed = Mat4(1.0f);
	};
}