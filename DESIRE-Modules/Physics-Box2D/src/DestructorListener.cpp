#include "DestructorListener.h"
#include "Joints/Joint2D.h"

#include "box2d/b2_joint.h"

void DestructorListener::SayGoodbye(b2Joint* joint)
{
	static_cast<Joint2D*>(joint->GetUserData())->OnJointDestroyed();
}

void DestructorListener::SayGoodbye(b2Fixture* fixture)
{
}
