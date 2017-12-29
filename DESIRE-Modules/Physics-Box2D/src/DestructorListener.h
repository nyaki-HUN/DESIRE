#pragma once

#include "Box2D/Dynamics/b2WorldCallbacks.h"

class DestructorListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Joint *joint) override;
	void SayGoodbye(b2Fixture *fixture) override;
};
