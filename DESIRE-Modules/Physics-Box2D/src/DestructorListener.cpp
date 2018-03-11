#include "DestructorListener.h"
#include "Joints/Joint2D.h"

#include "Box2D/Dynamics/Joints/b2Joint.h"

void DestructorListener::SayGoodbye(b2Joint *joint)
{
	static_cast<Joint2D*>(joint->GetUserData())->OnJointDestroyed();
}

void DestructorListener::SayGoodbye(b2Fixture *fixture)
{

}
