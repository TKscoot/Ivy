#pragma once
#include "Types.h"
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Ivy
{
	class Transform
	{
	public:
		inline Transform()
			: m_scale({ 1.0f, 1.0f, 1.0f })
			, m_rotation(Vec3())
			, m_translation({ 0.0f, 0.0f, 0.0f })
			, m_direction({ 0.0f, 0.0f, 1.0f })
			, m_right({ 1.0f, 0.0f, 0.0f })
			, m_up({ 0.0f, 1.0f, 0.0f })
			, m_composed(Mat4(1.0f))
		{
		}

		inline const Vec3&  getDirection() const { return m_direction; };
		inline const Vec3&  getUp()        const { return m_up; };
		inline const Vec3&  getRight()     const { return m_right; };
		inline const Vec3&  getRotation()  const { return m_rotation; }
		inline const Vec3&  getPosition()  const { return m_translation; }

		inline void setPositionX(const float& x) { m_translation.x = x; invalidate(); }
		inline void setPositionY(const float& y) { m_translation.y = y; invalidate(); }
		inline void setPositionZ(const float& z) { m_translation.z = z; invalidate(); }
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
		inline void setPosition(const Vec3& vec) { m_translation = vec; invalidate(); }

		inline void setRotationX(const float& x) { m_rotation.x = x; invalidate(); }
		inline void setRotationY(const float& y) { m_rotation.y = y; invalidate(); }
		inline void setRotationZ(const float& z) { m_rotation.z = z; invalidate(); }
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
		inline void setRotation(const Vec3& vec) { m_rotation = Vec4(vec, 1.0f); invalidate(); }
		inline void rotate(const Vec3& delta)
		{
			m_rotation += delta; invalidate();
		}

		inline void setScaleX(const float& factor) { m_scale.x = factor; invalidate(); }
		inline void setScaleY(const float& factor) { m_scale.y = factor; invalidate(); }
		inline void setScaleZ(const float& factor) { m_scale.z = factor; invalidate(); }
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
		inline void setScale(const Vec3& vec) { m_scale = Vec4(vec, 1.0f); invalidate(); }
		
		inline const Vec3& getScale() const { return m_scale; }

		inline const Mat4& getComposed() { invalidate(); return m_composed; }

		inline void invalidate()
		{
			//m_rotationMat = glm::eulerAngleXYZ(m_rotation.x, m_rotation.y, m_rotation.z);
			Mat4 rot = Mat4(1.0);
			Mat4 trans = Mat4(1.0);
			Mat4 scale = Mat4(1.0);
			rot = glm::rotate(rot, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			rot = glm::rotate(rot, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			rot = glm::rotate(rot, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			trans = glm::translate(trans, m_translation);
			scale = glm::scale(scale, m_scale);

			m_composed = scale * rot * trans;

		}

		//inline void Update() override {}


	private:
		Vec3  m_translation = Vec3(0.0f, 1.0f, 0.0f);
		Vec3  m_rotation = Vec3(0.0f, 1.0f, 0.0f);
		Vec3  m_scale = Vec3(1.0f, 1.0f, 1.0f);
		Mat4 m_rotationMat;

		Vec3  m_direction;
		Vec3  m_right;
		Vec3  m_up;

		Mat4 m_composed = Mat4(1.0f);
	};
}