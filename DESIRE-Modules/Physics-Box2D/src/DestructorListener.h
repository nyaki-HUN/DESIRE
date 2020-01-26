#pragma once

#include "box2d/b2_world_callbacks.h"

class DestructorListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Joint* joint) override;
	void SayGoodbye(b2Fixture* fixture) override;
};
