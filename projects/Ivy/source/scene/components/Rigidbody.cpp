#include "ivypch.h"
#include "Rigidbody.h"

Ivy::Rigidbody::Rigidbody(Ptr<Entity> entity, ColliderShape colliderShapeType, float mass, Vec3 localInertia)
	: Ivy::Component::Component(entity)
	, mMass(mass)
	, mLocalInertia(PhysicsHelper::Vec3toBtVector3(localInertia))
	, mColliderShapeType(colliderShapeType)
{
	mColliderTransform = PhysicsHelper::TransformToBtTransform(entity->GetComponent<Transform>());

	mIsDynamic = (mMass != 0.0f);

	
}

Ivy::Rigidbody::Rigidbody(Ptr<Entity> entity, Transform transform, float mass, Vec3 localInertia)
	: Ivy::Component::Component(entity)
	, mColliderTransform(PhysicsHelper::TransformToBtTransform(entity->GetComponent<Transform>()))
	, mMass(mass)
	, mLocalInertia(PhysicsHelper::Vec3toBtVector3(localInertia))
{
}

void Ivy::Rigidbody::OnSceneLoad(Ptr<Scene> scene)
{
	switch (mColliderShapeType)
	{
	case Ivy::BOX:
		mCollisionShape = CreatePtr<btBoxShape>(btVector3(1, 1, 1));
		break;
	case Ivy::SPHERE:
		mCollisionShape = CreatePtr<btSphereShape>(btScalar(1.0f));
		break;
	case Ivy::CONVEX_MESH:
		//mCollisionShape = CreatePtr<btBoxShape>(btVector3(1, 1, 1)); // TODO: implement mesh shape
		break;
	default:
		break;
	}

	if (mIsDynamic)
	{
		mCollisionShape->calculateLocalInertia(mMass, mLocalInertia);
	}


	mMotionState = CreatePtr<btDefaultMotionState>(mColliderTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mMass, mMotionState.get(), mCollisionShape.get(), mLocalInertia);
	mBtRigidbody = CreatePtr<btRigidBody>(rbInfo);

	scene->GetName();
}
