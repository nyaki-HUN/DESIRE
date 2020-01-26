#include "stdafx_Box2D.h"
#include "DestructorListener.h"
#include "Joints/Joint2D.h"

void DestructorListener::SayGoodbye(b2Joint* joint)
{
	static_cast<Joint2D*>(joint->GetUserData())->OnJointDestroyed();
}

void DestructorListener::SayGoodbye(b2Fixture* fixture)
{
}
