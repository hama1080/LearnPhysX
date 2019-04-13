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
	gFoundation
		= PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	// PVD�̐ݒ�
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport
		= PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(
		PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
	PxInitExtensions(*gPhysics, gPvd);

	// Scene�̍쐬
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(0);
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
}

// Dynamic Rigidbody�̍쐬
PxRigidDynamic* createDynamic(const PxTransform& t,
	const PxGeometry& geometry, PxMaterial& material, PxReal density = 10.0f)
{
	PxRigidDynamic* rigid_dynamic
		= PxCreateDynamic(*gPhysics, t, geometry, material, density);
	gScene->addActor(*rigid_dynamic);
	return rigid_dynamic;
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
	PxMaterial* const kMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// ���a1m�̋�������10m���痎�Ƃ�
	PxRigidDynamic* sphere = createDynamic(
		PxTransform((PxVec3(0.0f, 10.0f, 0.0f))),
		PxSphereGeometry(1.0f), *kMaterial);

	for (PxU32 i = 0; i != kMaxSimulationStep; i++) {
		PxVec3 p = sphere->getGlobalPose().p;
		cout << "Step: " << i <<
			", Position: (" << p.x << ", " << p.y << ", " << p.z << ")" << endl;
		stepPhysics();
	}

	cout << "End simulation" << endl;
	int tmp;
	cin >> tmp;
	return 0;
}