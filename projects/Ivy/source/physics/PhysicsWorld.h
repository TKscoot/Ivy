#pragma once
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include "Types.h"
#include "scene/components/Transform.h"

namespace Ivy
{
	enum ColliderShape { BOX, SPHERE, CONVEX_MESH };

	namespace PhysicsHelper
	{
		inline Vec3 btVector3toVec3(btVector3 v)
		{
			return { v.x(), v.y(), v.z() };
		}

		inline btVector3 Vec3toBtVector3(Vec3 v)
		{
			return { v.x, v.y, v.z };
		}

		inline btTransform TransformToBtTransform(Ptr<Transform> t)
		{
			btTransform ct;
			ct.setIdentity();
			ct.setOrigin(Vec3toBtVector3(t->getPosition()));
			btQuaternion q;
			q.setEulerZYX(t->getRotation().z, t->getRotation().y, t->getRotation().x); // TODO: weiﬂ nicht, ob das so stimmt mit XYZ
			ct.setRotation(q);

			return ct;
		}
	}
	

	class PhysicsWorld
	{
	public:
		PhysicsWorld();

		void Initialize();

		void Update();

	private:
		Ptr<btDefaultCollisionConfiguration> mCollisionConfig = nullptr;
		Ptr<btCollisionDispatcher> mCollisionDispatcher = nullptr;
		Ptr<btBroadphaseInterface> mDbvtBroadphase = nullptr;
		Ptr<btSequentialImpulseConstraintSolver> mConstraintSolver = nullptr;
		Ptr<btDiscreteDynamicsWorld> mDynamicsWorld = nullptr;

		btAlignedObjectArray<Ptr<btCollisionShape>> mCollisionShapes = {};
	};
}
