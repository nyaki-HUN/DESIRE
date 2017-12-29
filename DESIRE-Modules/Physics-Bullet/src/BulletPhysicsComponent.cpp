#include "stdafx.h"
#include "BulletPhysicsComponent.h"
#include "BulletPhysics.h"
#include "BulletVectormathExt.h"

#include "Core/math/AABB.h"

#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"

BulletPhysicsComponent::BulletPhysicsComponent(bool dynamic)
	: body(nullptr)
	, shape(nullptr)
	, motionState(nullptr)
	, triangleIndexVertexArrays(nullptr)
	, dynamic(dynamic)
{
	int *indices = nullptr;
	float *vertices = nullptr;

	uint32_t numIndices = 0;
	uint32_t numVertices = 0;

	int stride = 3;

	btVector3 localInertia(0.0f, 0.0f, 0.0f);

	if(dynamic)
	{
		AABB aabb(Vector3(FLT_MAX), Vector3(-FLT_MAX));
		for(uint32_t i = 0; i < numVertices; i++)
		{
			aabb.AddPoint(Vector3(vertices[i * stride + 0], vertices[i * stride + 1], vertices[i * stride + 2]));
		}

		if(false)
		{
			shape = new btBoxShape(GetBtVector3(aabb.GetSize() * 0.5f));
		}
		else
		{
			const btVector3 cmassVec = GetBtVector3(aabb.GetCenter());

			btConvexHullShape *convexHullShape = new btConvexHullShape();
			for(uint32_t i = 0; i < numIndices; i++)
			{
				btVector3 v(vertices[indices[i] * stride], vertices[indices[i] * stride + 1], vertices[indices[i] * stride + 2]);
				v -= cmassVec;
				convexHullShape->addPoint(v, false);
			}
			convexHullShape->recalcLocalAabb();

			shape = convexHullShape;
		}

		shape->calculateLocalInertia(10.0f, localInertia);
	}
	else
	{
		btIndexedMesh mesh;
		mesh.m_numTriangles = (int)numIndices / 3;
		mesh.m_triangleIndexBase = (uint8_t*)indices;
		mesh.m_triangleIndexStride = 3 * sizeof(int);
		mesh.m_numVertices = (int)numVertices;
		mesh.m_vertexBase = (uint8_t*)vertices;
		mesh.m_vertexStride = (int)(stride * sizeof(float));

		triangleIndexVertexArrays = new btTriangleIndexVertexArray();
		triangleIndexVertexArrays->addIndexedMesh(mesh);

		btBvhTriangleMeshShape *trimeshShape = new btBvhTriangleMeshShape(triangleIndexVertexArrays, true);
		btTriangleInfoMap *triangleInfoMap = new btTriangleInfoMap();
		btGenerateInternalEdgeInfo(trimeshShape, triangleInfoMap);

		shape = trimeshShape;
	}

	motionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo cInfo(0.0f, motionState, shape, localInertia);
	body = new btRigidBody(cInfo);
	body->setUserPointer(this);

	BulletPhysics *physics = static_cast<BulletPhysics*>(IPhysics::Get());
	physics->dynamicsWorld->addRigidBody(body, (1 << (int)collisionLayer), physics->GetMaskForCollisionLayer(collisionLayer));

	if(dynamic)
	{
		SetCollisionLayer(EPhysicsCollisionLayer::DYNAMIC);
		body->setActivationState(ISLAND_SLEEPING);
	}
	else
	{
		SetCollisionLayer(EPhysicsCollisionLayer::DEFAULT);
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

BulletPhysicsComponent::~BulletPhysicsComponent()
{
	BulletPhysics *physics = static_cast<BulletPhysics*>(IPhysics::Get());
	physics->dynamicsWorld->removeRigidBody(body);
	delete body;

	if(!dynamic)
	{
		btBvhTriangleMeshShape *trimeshShape = static_cast<btBvhTriangleMeshShape*>(shape);
		btTriangleInfoMap *triangleInfoMap = trimeshShape->getTriangleInfoMap();
		delete triangleInfoMap;
		trimeshShape->setTriangleInfoMap(nullptr);
	}
	
	delete shape;
	delete motionState;
	delete triangleIndexVertexArrays;
}

void BulletPhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer i_collisionLayer)
{
	if(collisionLayer == i_collisionLayer)
	{
		return;
	}

	collisionLayer = i_collisionLayer;

	if(collisionLayer == EPhysicsCollisionLayer::NO_COLLISION)
	{
		body->forceActivationState(DISABLE_SIMULATION);
	}
	else if(body->getActivationState() == DISABLE_SIMULATION)
	{
		body->forceActivationState(ISLAND_SLEEPING);
	}

	BulletPhysics *physics = static_cast<BulletPhysics*>(IPhysics::Get());
	btBroadphaseProxy *handle = body->getBroadphaseHandle();
	handle->m_collisionFilterGroup = (1 << (int)collisionLayer);
	handle->m_collisionFilterMask = physics->GetMaskForCollisionLayer(collisionLayer);
}

void BulletPhysicsComponent::SetMass(float mass)
{
	if(body->isStaticOrKinematicObject() == false)
	{
		btVector3 localInertia;
		body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
		body->setMassProps(mass, localInertia);
	}
}

float BulletPhysicsComponent::GetMass() const
{
	const float rv = body->getInvMass();
	return (rv != 0.0f) ? 1.0f / rv : 0.0f;
}
