#pragma once
#include "Types.h"
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "scene/components/Component.h"

namespace Ivy
{
	class Transform : public Component
	{
	public:
		Transform()
			: mScale({ 1.0f, 1.0f, 1.0f })
			, mRotation(Vec3())
			, mTranslation({ 0.0f, 0.0f, 0.0f })
			, mDirection({ 0.0f, 0.0f, 1.0f })
			, mRight({ 1.0f, 0.0f, 0.0f })
			, mUp({ 0.0f, 1.0f, 0.0f })
			, mComposed(Mat4(1.0f))
		{
		}

		inline const Vec3&  getDirection() const { return mDirection; };
		inline const Vec3&  getUp()        const { return mUp; };
		inline const Vec3&  getRight()     const { return mRight; };
		inline const Vec3&  getRotation()  const { return mRotation; }
		inline const Vec3&  getPosition()  const { return mTranslation; }

		inline void setPositionX(const float& x) { mTranslation.x = x; invalidate(); }
		inline void setPositionY(const float& y) { mTranslation.y = y; invalidate(); }
		inline void setPositionZ(const float& z) { mTranslation.z = z; invalidate(); }
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
		inline void setPosition(const Vec3& vec) { mTranslation = vec; invalidate(); }

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
			trans = glm::translate(trans, mTranslation);
			scale = glm::scale(scale, mScale);

			//mComposed = scale * rot * trans;
			mComposed = trans * scale * rot;

		}



	private:
		Vec3  mTranslation = Vec3(0.0f, 1.0f, 0.0f);
		Vec3  mRotation	= Vec3(0.0f, 1.0f, 0.0f);
		Vec3  mScale		= Vec3(1.0f, 1.0f, 1.0f);
		Mat4  mRotationMat = Mat4();

		Vec3  mDirection;
		Vec3  mRight;
		Vec3  mUp;

		Mat4 mComposed = Mat4(1.0f);
	};
}