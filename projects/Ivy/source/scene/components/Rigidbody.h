#pragma once
#include "scene/Scene.h"
#include "scene/Entity.h"
#include "Component.h"
#include "physics/PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace Ivy
{
	class Rigidbody : public Component
	{
	public:
		Rigidbody(Ptr<Entity> entity, ColliderShape colliderShapeType = BOX, float mass = 1.0f, Vec3 localInertia = { 0.0f, 0.0f, 0.0f });
		Rigidbody(Ptr<Entity> entity, Transform transform, float mass, Vec3 localInertia);

		void OnSceneLoad(Ptr<Scene> scene) override;

	private:
		Ptr<btCollisionShape>	  mCollisionShape = nullptr;
		Ptr<btDefaultMotionState> mMotionState    = nullptr;
		Ptr<btRigidBody>		  mBtRigidbody    = nullptr;

		btTransform mColliderTransform;

		ColliderShape mColliderShapeType = BOX;

		float mMass = 1.0f;

		bool mIsDynamic = false;

		btVector3 mLocalInertia = { 0, 0, 0 };
	};
}
