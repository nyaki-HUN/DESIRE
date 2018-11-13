/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "Box2D/Common/b2Settings.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// Collision
float b2_linearSlop				= 0.005f;
float b2_angularSlop			= (2.0f / 180.0f * b2_pi);
// Dynamics
float b2_velocityThreshold		= 1.0f;
float b2_maxLinearCorrection	= 0.2f;
float b2_maxAngularCorrection	= (8.0f / 180.0f * b2_pi);
float b2_baumgarte				= 0.2f;
float b2_toiBaugarte			= 0.75f;
// Sleep
float b2_timeToSleep			= 0.5f;
float b2_linearSleepTolerance	= 0.01f;
float b2_angularSleepTolerance	= (2.0f / 180.0f * b2_pi);

b2Version b2_version = {2, 3, 2};

// Memory allocators. Modify these to use your own allocator.
void* b2Alloc(int32 size)
{
	return malloc(size);
}

void b2Free(void* mem)
{
	free(mem);
}

// You can modify this to use your logging facility.
void b2Log(const char* string, ...)
{
	va_list args;
	va_start(args, string);
	vprintf(string, args);
	va_end(args);
}
