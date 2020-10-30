#include "stdafx_Bullet.h"
#include "BulletPhysicsComponent.h"

#include "BulletPhysics.h"
#include "BulletVectormathExt.h"

#include "Engine/Core/Math/AABB.h"
#include "Engine/Core/Math/Transform.h"
#include "Engine/Core/Object.h"

#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"

BulletPhysicsComponent::BulletPhysicsComponent(Object& object, bool dynamic)
	: PhysicsComponent(object)
	, m_dynamic(dynamic)
{
	int* pIndices = nullptr;
	float* pVertices = nullptr;

	uint32_t numIndices = 0;
	uint32_t numVertices = 0;

	int stride = 3;

	btCollisionShape* pCollisionShape = nullptr;
	btVector3 localInertia(0.0f, 0.0f, 0.0f);

	if(dynamic)
	{
		AABB aabb(Vector3(FLT_MAX), Vector3(-FLT_MAX));
		for(uint32_t i = 0; i < numVertices; i++)
		{
			aabb.AddPoint(Vector3(pVertices[i * stride + 0], pVertices[i * stride + 1], pVertices[i * stride + 2]));
		}

		if(false)
		{
			pCollisionShape = new btBoxShape(GetBtVector3(aabb.GetSize() * 0.5f));
		}
		else
		{
			const btVector3 cmassVec = GetBtVector3(aabb.GetCenter());

			btConvexHullShape* pConvexHullShape = new btConvexHullShape();
			for(uint32_t i = 0; i < numIndices; ++i)
			{
				btVector3 v(pVertices[pIndices[i] * stride], pVertices[pIndices[i] * stride + 1], pVertices[pIndices[i] * stride + 2]);
				v -= cmassVec;
				pConvexHullShape->addPoint(v, false);
			}
			pConvexHullShape->recalcLocalAabb();

			pCollisionShape = pConvexHullShape;
		}

		pCollisionShape->calculateLocalInertia(10.0f, localInertia);
	}
	else
	{
		btIndexedMesh mesh;
		mesh.m_numTriangles = static_cast<int>(numIndices) / 3;
		mesh.m_triangleIndexBase = reinterpret_cast<uint8_t*>(pIndices);
		mesh.m_triangleIndexStride = 3 * sizeof(int);
		mesh.m_numVertices = static_cast<int>(numVertices);
		mesh.m_vertexBase = reinterpret_cast<uint8_t*>(pVertices);
		mesh.m_vertexStride = static_cast<int>(stride * sizeof(float));

		m_pTriangleIndexVertexArrays = new btTriangleIndexVertexArray();
		m_pTriangleIndexVertexArrays->addIndexedMesh(mesh);

		btBvhTriangleMeshShape* pTrimeshShape = new btBvhTriangleMeshShape(m_pTriangleIndexVertexArrays, true);
		btTriangleInfoMap* pTriangleInfoMap = new btTriangleInfoMap();
		btGenerateInternalEdgeInfo(pTrimeshShape, pTriangleInfoMap);

		pCollisionShape = pTrimeshShape;
	}

	m_pMotionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo cInfo(0.0f, m_pMotionState, pCollisionShape, localInertia);
	cInfo.m_friction = m_physicsMaterial.friction;
	cInfo.m_restitution = m_physicsMaterial.bounciness;
	m_pBody = new btRigidBody(cInfo);
	m_pBody->setUserPointer(this);

	btDynamicsWorld& world = static_cast<BulletPhysics*>(Modules::Physics.get())->GetWorld();
	world.addRigidBody(m_pBody, 1 << static_cast<int>(m_collisionLayer), Modules::Physics->GetMaskForCollisionLayer(m_collisionLayer));

	if(dynamic)
	{
		SetCollisionLayer(EPhysicsCollisionLayer::Dynamic);
		m_pBody->setActivationState(ISLAND_SLEEPING);
	}
	else
	{
		SetCollisionLayer(EPhysicsCollisionLayer::Default);
		m_pBody->setCollisionFlags(m_pBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

BulletPhysicsComponent::~BulletPhysicsComponent()
{
	btCollisionShape* pCollisionShape = m_pBody->getCollisionShape();

	btDynamicsWorld& world = static_cast<BulletPhysics*>(Modules::Physics.get())->GetWorld();
	world.removeRigidBody(m_pBody);
	delete m_pBody;

	if(!m_dynamic)
	{
		btBvhTriangleMeshShape* pTrimeshShape = static_cast<btBvhTriangleMeshShape*>(pCollisionShape);
		btTriangleInfoMap* pTriangleInfoMap = pTrimeshShape->getTriangleInfoMap();
		delete pTriangleInfoMap;
		pTrimeshShape->setTriangleInfoMap(nullptr);
	}

	delete pCollisionShape;
	delete m_pMotionState;
	delete m_pTriangleIndexVertexArrays;
}

void BulletPhysicsComponent::SetEnabled(bool value)
{
	if(IsEnabled() == value)
	{
		return;
	}

	PhysicsComponent::SetEnabled(value);
	m_pBody->forceActivationState(value ? ISLAND_SLEEPING : DISABLE_SIMULATION);
}

void BulletPhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer collisionLayer)
{
	m_collisionLayer = collisionLayer;

	if(m_collisionLayer == EPhysicsCollisionLayer::NoCollision)
	{
		m_pBody->forceActivationState(DISABLE_SIMULATION);
	}
	else if(m_pBody->getActivationState() == DISABLE_SIMULATION)
	{
		m_pBody->forceActivationState(ISLAND_SLEEPING);
	}

	btBroadphaseProxy* pHandle = m_pBody->getBroadphaseHandle();
	pHandle->m_collisionFilterGroup = 1 << static_cast<int>(m_collisionLayer);
	pHandle->m_collisionFilterMask = Modules::Physics->GetMaskForCollisionLayer(m_collisionLayer);
}

void BulletPhysicsComponent::SetCollisionDetectionMode(ECollisionDetectionMode mode)
{
	switch(mode)
	{
		case ECollisionDetectionMode::Discrete:
			m_pBody->setCcdMotionThreshold(0.0f);
			m_pBody->setCcdSweptSphereRadius(0.0f);
			break;

		case ECollisionDetectionMode::Continuous:
			// Enable CCD if the object moves more than 0.01 meter in one simulation frame
			m_pBody->setCcdMotionThreshold(0.01);
			m_pBody->setCcdSweptSphereRadius(0.5f);
			break;
	}
}

PhysicsComponent::ECollisionDetectionMode BulletPhysicsComponent::GetCollisionDetectionMode() const
{
	return (m_pBody->getCcdMotionThreshold() > 0.0f) ? ECollisionDetectionMode::Continuous : ECollisionDetectionMode::Discrete;
}

Array<PhysicsComponent*> BulletPhysicsComponent::GetActiveCollidingComponents() const
{
	Array<PhysicsComponent*> collisions;
	DESIRE_TODO("Implement GetActiveCollidingComponents()");
	return collisions;
}

void BulletPhysicsComponent::SetBodyType(EBodyType bodyType)
{
	int flags = m_pBody->getCollisionFlags();

	// Remove flags
	flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
	flags &= ~btCollisionObject::CF_STATIC_OBJECT;

	switch(bodyType)
	{
		case EBodyType::Static:		flags |= btCollisionObject::CF_STATIC_OBJECT; break;
		case EBodyType::Dynamic:	break;
		case EBodyType::Kinematic:	flags |= btCollisionObject::CF_KINEMATIC_OBJECT; break;
	}

	m_pBody->setCollisionFlags(flags);
}

PhysicsComponent::EBodyType BulletPhysicsComponent::GetBodyType() const
{
	if(m_pBody->isStaticObject())
	{
		return PhysicsComponent::EBodyType::Static;
	}

	if(m_pBody->isKinematicObject())
	{
		return PhysicsComponent::EBodyType::Kinematic;
	}

	return PhysicsComponent::EBodyType::Dynamic;
}

void BulletPhysicsComponent::SetTrigger(bool value)
{
	DESIRE_TODO("Implement SetTrigger()");
}

bool BulletPhysicsComponent::IsTrigger() const
{
	DESIRE_TODO("Implement IsTrigger()");
	return false;
}

void BulletPhysicsComponent::SetMass(float mass)
{
	if(m_pBody->isStaticOrKinematicObject() == false)
	{
		btVector3 localInertia;
		m_pBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
		m_pBody->setMassProps(mass, localInertia);
	}
}

float BulletPhysicsComponent::GetMass() const
{
	const float rv = m_pBody->getInvMass();
	return (rv != 0.0f) ? 1.0f / rv : 0.0f;
}

Vector3 BulletPhysicsComponent::GetCenterOfMass() const
{
	return GetVector3(m_pBody->getCenterOfMassPosition());
}

void BulletPhysicsComponent::SetLinearDamping(float value)
{
	m_pBody->setDamping(value, m_pBody->getAngularDamping());
}

float BulletPhysicsComponent::GetLinearDamping() const
{
	return m_pBody->getLinearDamping();
}

void BulletPhysicsComponent::SetAngularDamping(float value)
{
	m_pBody->setDamping(m_pBody->getLinearDamping(), value);
}

float BulletPhysicsComponent::GetAngularDamping() const
{
	return m_pBody->getAngularDamping();
}

void BulletPhysicsComponent::SetLinearVelocity(const Vector3& linearVelocity)
{
	m_pBody->setLinearVelocity(GetBtVector3(linearVelocity));
}

Vector3 BulletPhysicsComponent::GetLinearVelocity() const
{
	return GetVector3(m_pBody->getLinearVelocity());
}

void BulletPhysicsComponent::SetAngularVelocity(const Vector3& angularVelocity)
{
	m_pBody->setAngularVelocity(GetBtVector3(angularVelocity));
}

Vector3 BulletPhysicsComponent::GetAngularVelocity() const
{
	return GetVector3(m_pBody->getAngularVelocity());
}

void BulletPhysicsComponent::AddForce(const Vector3& force, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		m_pBody->applyCentralForce(GetBtVector3(force)); break;
		case EForceMode::Impulse:	m_pBody->applyCentralImpulse(GetBtVector3(force)); break;
	}
}

void BulletPhysicsComponent::AddForceAtPosition(const Vector3& force, const Vector3& position, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		m_pBody->applyForce(GetBtVector3(force), GetBtVector3(position));
		case EForceMode::Impulse:	m_pBody->applyImpulse(GetBtVector3(force), GetBtVector3(position));
	}
}

void BulletPhysicsComponent::AddTorque(const Vector3& torque, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		m_pBody->applyTorque(GetBtVector3(torque)); break;
		case EForceMode::Impulse:	m_pBody->applyTorqueImpulse(GetBtVector3(torque)); break;
	}
}

void BulletPhysicsComponent::SetLinearMotionLock(bool axisX, bool axisY, bool axisZ)
{
	m_pBody->setLinearFactor(btVector3(
		axisX ? 1.0f : 0.0f,
		axisY ? 1.0f : 0.0f,
		axisZ ? 1.0f : 0.0f
	));
}

void BulletPhysicsComponent::SetAngularMotionLock(bool axisX, bool axisY, bool axisZ)
{
	m_pBody->setAngularFactor(btVector3(
		axisX ? 1.0f : 0.0f,
		axisY ? 1.0f : 0.0f,
		axisZ ? 1.0f : 0.0f
	));
}

bool BulletPhysicsComponent::IsSleeping() const
{
	return (m_pBody->getActivationState() == ISLAND_SLEEPING);
}

void BulletPhysicsComponent::UpdateGameObjectTransform() const
{
	Transform& transform = object.GetTransform();
	const btTransform& btTransform = m_pBody->getWorldTransform();
	transform.SetPosition(GetVector3(btTransform.getOrigin()));
	transform.SetRotation(GetQuat(btTransform.getRotation()));

}

void BulletPhysicsComponent::SetTransformFromGameObject()
{
	const Transform& transform = object.GetTransform();
	btTransform& btTransform = m_pBody->getWorldTransform();
	btTransform.setOrigin(GetBtVector3(transform.GetPosition()));
	btTransform.setRotation(GetBtQuat(transform.GetRotation()));
}
