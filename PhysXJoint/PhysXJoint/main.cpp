#include <iostream>
#include "PxPhysicsAPI.h"

using namespace std;
using namespace physx;

PxDefaultAllocator      gAllocator;
PxDefaultErrorCallback  gErrorCallback;
PxFoundation*           gFoundation = NULL;
PxPhysics*              gPhysics = NULL;
PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene*                gScene = NULL;
PxPvd*                  gPvd = NULL;

// PhysX�̏�����
void initPhysics()
{
	gFoundation =
		PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	// PVD�̐ݒ�
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport =
		PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(
		PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	// Scene�̍쐬
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	// PVD�̐ݒ�
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	PxInitExtensions(*gPhysics, gPvd);
}

// Dynamic Rigidbody�̍쐬
PxRigidDynamic* createDynamic(const PxTransform& t,
	const PxGeometry& geometry, PxMaterial& material)
{
	const PxReal kDensity = 10.0f;
	PxRigidDynamic* dynamic
		= PxCreateDynamic(*gPhysics, t, geometry, material, kDensity);
	gScene->addActor(*dynamic);
	return dynamic;
}

// Static Rigidbody�̍쐬
PxRigidStatic* createStatic(const PxTransform& t, const PxGeometry& geometry, PxMaterial& material)
{
	PxRigidStatic* static_actor = PxCreateStatic(*gPhysics, t, geometry, material);
	gScene->addActor(*static_actor);
	return static_actor;
}


// �V�~�����[�V�����X�e�b�v��i�߂�
void stepPhysics()
{
	const PxReal kElapsedTime = 1.0f / 60.0f; // 60Hz
	gScene->simulate(kElapsedTime);
	gScene->fetchResults(true);
}

int main(void)
{
	initPhysics();
	cout << "PhysXHelloWorld" << endl;
	cout << "Start simulation" << endl;

	const PxU32 kMaxSimulationStep = 100;

	// �Ö��C�W���A�����C�W���A�����W���̏�
	PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// base plate(12m x 0.2m x 10m)
	const PxVec3 kPlateHalfExtents(6.0f, 0.1f, 5.0f);
	createStatic(
		PxTransform(PxVec3(0.0f, 0.0f, 0.0f)),
		PxBoxGeometry(kPlateHalfExtents.x, kPlateHalfExtents.y, kPlateHalfExtents.z),
		*material);


	// 2�̍��̂��쐬
	const PxReal kHeight = 5.0f;
	PxBoxGeometry box0(PxVec3(1.0f, 0.1f, 0.2f));
	PxBoxGeometry box1(PxVec3(0.1f, 0.4f, 0.2f));

	PxRigidDynamic* actor0 =
		createDynamic(
			PxTransform((PxVec3(0.0f, kHeight, 0.0f))),
			box0,
			*material);
	PxRigidDynamic* actor1 =
		createDynamic(
			PxTransform((PxVec3(0.9f, kHeight + 0.5f, 0.0f))),
			box1,
			*material);

	// �W���C���g�ɂ�荄�̂�A��
	PxVec3 jointPos = PxVec3(0.9f, kHeight + 0.1f, 0.0f);

	PxFixedJoint* joint = 
		PxFixedJointCreate(*gPhysics,
		actor0,
		PxTransform(jointPos - actor0->getGlobalPose().p),
		actor1,
		PxTransform(jointPos - actor1->getGlobalPose().p)
	);

	// �W���C���g�̔j�f�ݒ�
	const PxReal kBreakForce = 100.0f;
	const PxReal kBreakTorque = 100.0f;
	joint->setBreakForce(kBreakForce, kBreakTorque);


	// simulation loop
	for (PxU32 i = 0; i != 500; i++)
	{
		if (i % 100 == 0)
			cout << "Simulation step: " << i << endl;
		stepPhysics();
	}

	cout << "End simulation" << endl;
	int tmp;
	cin >> tmp;
	return 0;
}