#include "ivypch.h"
#include "PhysicsWorld.h"

Ivy::PhysicsWorld::PhysicsWorld()
{
	Initialize();
}

void Ivy::PhysicsWorld::Initialize()
{
	//collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	mCollisionConfig = CreatePtr<btDefaultCollisionConfiguration>();

	//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mCollisionDispatcher = CreatePtr<btCollisionDispatcher>(mCollisionConfig.get());

	//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mDbvtBroadphase = CreatePtr<btDbvtBroadphase>();

	//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mConstraintSolver = CreatePtr<btSequentialImpulseConstraintSolver>();

	mDynamicsWorld = CreatePtr<btDiscreteDynamicsWorld>(
						mCollisionDispatcher.get(),
						mDbvtBroadphase.get(),
						mConstraintSolver.get(),
						mCollisionConfig.get());

	mDynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
}

void Ivy::PhysicsWorld::Update()
{
	mDynamicsWorld->stepSimulation(1.f / 60.f, 10);

	//print positions of all objects
	for (int j = mDynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
	{
		btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[j];
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform trans;
		if (body && body->getMotionState())
		{
			body->getMotionState()->getWorldTransform(trans);
		}
		else
		{
			trans = obj->getWorldTransform();
		}
		printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
	}
}
