#include "BulletPhysicsComponent.h"
#include "BulletPhysics.h"
#include "BulletVectormathExt.h"

#include "Engine/Core/Modules.h"
#include "Engine/Core/math/AABB.h"
#include "Engine/Scene/Object.h"
#include "Engine/Scene/Transform.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"

BulletPhysicsComponent::BulletPhysicsComponent(Object& object, bool dynamic)
	: PhysicsComponent(object)
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
	cInfo.m_friction = physicsMaterial.friction;
	cInfo.m_restitution = physicsMaterial.bounciness;
	body = new btRigidBody(cInfo);
	body->setUserPointer(this);

	btDynamicsWorld *world = static_cast<BulletPhysics*>(Modules::Physics.get())->GetWorld();
	world->addRigidBody(body, 1 << (int)collisionLayer, Modules::Physics->GetMaskForCollisionLayer(collisionLayer));

	if(dynamic)
	{
		SetCollisionLayer(EPhysicsCollisionLayer::Dynamic);
		body->setActivationState(ISLAND_SLEEPING);
	}
	else
	{
		SetCollisionLayer(EPhysicsCollisionLayer::Default);
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

BulletPhysicsComponent::~BulletPhysicsComponent()
{
	btDynamicsWorld *world = static_cast<BulletPhysics*>(Modules::Physics.get())->GetWorld();
	world->removeRigidBody(body);
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
	collisionLayer = i_collisionLayer;

	if(collisionLayer == EPhysicsCollisionLayer::NoCollision)
	{
		body->forceActivationState(DISABLE_SIMULATION);
	}
	else if(body->getActivationState() == DISABLE_SIMULATION)
	{
		body->forceActivationState(ISLAND_SLEEPING);
	}

	btBroadphaseProxy *handle = body->getBroadphaseHandle();
	handle->m_collisionFilterGroup = 1 << (int)collisionLayer;
	handle->m_collisionFilterMask = Modules::Physics->GetMaskForCollisionLayer(collisionLayer);
}

void BulletPhysicsComponent::SetCollisionDetectionMode(ECollisionDetectionMode mode)
{
	switch(mode)
	{
		case ECollisionDetectionMode::Discrete:
			body->setCcdMotionThreshold(0.0f);
			body->setCcdSweptSphereRadius(0.0f);
			break;

		case ECollisionDetectionMode::Continuous:
			// Enable CCD if the object moves more than 0.01 meter in one simulation frame
			body->setCcdMotionThreshold(0.01);
			body->setCcdSweptSphereRadius(0.5f);
			break;
	}
}

PhysicsComponent::ECollisionDetectionMode BulletPhysicsComponent::GetCollisionDetectionMode() const
{
	return (body->getCcdMotionThreshold() > 0.0f) ? ECollisionDetectionMode::Continuous : ECollisionDetectionMode::Discrete;
}

std::vector<PhysicsComponent*> BulletPhysicsComponent::GetActiveCollidingComponents() const
{
	std::vector<PhysicsComponent*> collisions;
	DESIRE_TODO("Implement GetActiveCollidingComponents()");
	return collisions;
}

void BulletPhysicsComponent::SetBodyType(EBodyType bodyType)
{
	int flags = body->getCollisionFlags();

	// Remove flags
	flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
	flags &= ~btCollisionObject::CF_STATIC_OBJECT;

	switch(bodyType)
	{
		case EBodyType::Static:		flags |= btCollisionObject::CF_STATIC_OBJECT; break;
		case EBodyType::Dynamic:	break;
		case EBodyType::Kinematic:	flags |= btCollisionObject::CF_KINEMATIC_OBJECT; break;
	}

	body->setCollisionFlags(flags);
}

PhysicsComponent::EBodyType BulletPhysicsComponent::GetBodyType() const
{
	if(body->isStaticObject())
	{
		return PhysicsComponent::EBodyType::Static;
	}

	if(body->isKinematicObject())
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

Vector3 BulletPhysicsComponent::GetCenterOfMass() const
{
	return GetVector3(body->getCenterOfMassPosition());
}

void BulletPhysicsComponent::SetLinearDamping(float value)
{
	body->setDamping(value, body->getAngularDamping());
}

float BulletPhysicsComponent::GetLinearDamping() const
{
	return body->getLinearDamping();
}

void BulletPhysicsComponent::SetAngularDamping(float value)
{
	body->setDamping(body->getLinearDamping(), value);
}

float BulletPhysicsComponent::GetAngularDamping() const
{
	return body->getAngularDamping();
}

void BulletPhysicsComponent::SetLinearVelocity(const Vector3& linearVelocity)
{
	body->setLinearVelocity(GetBtVector3(linearVelocity));
}

Vector3 BulletPhysicsComponent::GetLinearVelocity() const
{
	return GetVector3(body->getLinearVelocity());
}

void BulletPhysicsComponent::SetAngularVelocity(const Vector3& angularVelocity)
{
	body->setAngularVelocity(GetBtVector3(angularVelocity));
}

Vector3 BulletPhysicsComponent::GetAngularVelocity() const
{
	return GetVector3(body->getAngularVelocity());
}

void BulletPhysicsComponent::AddForce(const Vector3& force, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		body->applyCentralForce(GetBtVector3(force)); break;
		case EForceMode::Impulse:	body->applyCentralImpulse(GetBtVector3(force)); break;
	}
}

void BulletPhysicsComponent::AddForceAtPosition(const Vector3& force, const Vector3& position, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		body->applyForce(GetBtVector3(force), GetBtVector3(position));
		case EForceMode::Impulse:	body->applyImpulse(GetBtVector3(force), GetBtVector3(position));
	}
}

void BulletPhysicsComponent::AddTorque(const Vector3& torque, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		body->applyTorque(GetBtVector3(torque)); break;
		case EForceMode::Impulse:	body->applyTorqueImpulse(GetBtVector3(torque)); break;
	}
}

bool BulletPhysicsComponent::IsSleeping() const
{
	return (body->getActivationState() == ISLAND_SLEEPING);
}

void BulletPhysicsComponent::UpdateGameObjectTransform() const
{
	Transform& transform = object.GetTransform();
	const btTransform& btTransform = body->getWorldTransform();
	transform.SetPosition(GetVector3(btTransform.getOrigin()));
	transform.SetRotation(GetQuat(btTransform.getRotation()));

}

void BulletPhysicsComponent::SetTransformFromGameObject()
{
	const Transform& transform = object.GetTransform();
	btTransform& btTransform = body->getWorldTransform();
	btTransform.setOrigin(GetBtVector3(transform.GetPosition()));
	btTransform.setRotation(GetBtQuat(transform.GetRotation()));
}

void BulletPhysicsComponent::SetEnabled(bool value)
{
	if(IsEnabled() == value)
	{
		return;
	}

	PhysicsComponent::SetEnabled(value);
	body->forceActivationState(value ? ISLAND_SLEEPING : DISABLE_SIMULATION);
}
