#include "Joints/Joint2D.h"
#include "Box2DPhysics.h"
#include "Box2DPhysicsComponent.h"
#include "b2MathExt.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/Modules.h"

#include "Box2D/Dynamics/Joints/b2Joint.h"
#include "Box2D/Dynamics/b2World.h"

Joint2D::Joint2D()
{

}

Joint2D::~Joint2D()
{
	DestroyJoint();
}

void Joint2D::SetCollisionEnabled(bool value)
{
	GetJointDef().collideConnected = value;
	CreateJoint();
}

bool Joint2D::IsCollisionEnabled() const
{
	return GetJointDef().collideConnected;
}

Vector2 Joint2D::GetReactionForce(float timeStep) const
{
	ASSERT(joint != nullptr);
	return GetVector2(joint->GetReactionForce(timeStep));
}

float Joint2D::GetReactionTorque(float timeStep) const
{
	ASSERT(joint != nullptr);
	return joint->GetReactionTorque(timeStep);
}

void Joint2D::CreateJointBetween(Box2DPhysicsComponent *anchoredComponent, Box2DPhysicsComponent *connectedComponent)
{
	if(anchoredComponent == nullptr)
	{
		return;
	}

	GetJointDef().bodyA = anchoredComponent->GetBody();
	GetJointDef().bodyB = (connectedComponent != nullptr) ? connectedComponent->GetBody() : static_cast<Box2DPhysics*>(Modules::Physics.get())->GetWorldBody();
	CreateJoint();
}

void Joint2D::OnJointDestroyed()
{
	joint = nullptr;
}

void Joint2D::CreateJoint()
{
	DestroyJoint();

	const b2JointDef& jointDef = GetJointDef();
	if(jointDef.bodyA == nullptr || jointDef.bodyB == nullptr)
	{
		return;
	}

	b2World *world = static_cast<Box2DPhysics*>(Modules::Physics.get())->GetWorld();
	joint = world->CreateJoint(&jointDef);
}

void Joint2D::DestroyJoint()
{
	if(joint != nullptr)
	{
		b2World *world = static_cast<Box2DPhysics*>(Modules::Physics.get())->GetWorld();
		world->DestroyJoint(joint);
	}
}
