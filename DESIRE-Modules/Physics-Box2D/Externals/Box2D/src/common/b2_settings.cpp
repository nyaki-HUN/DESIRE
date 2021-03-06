// MIT License

// Copyright (c) 2019 Erin Catto

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "box2d/b2_settings.h"
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

b2Version b2_version = {2, 4, 0};

/*
// Memory allocators. Modify these to use your own allocator.
void* b2Alloc(int32 size)
{
	return malloc(size);
}

void b2Free(void* mem)
{
	free(mem);
}
*/

// You can modify this to use your logging facility.
void b2Log(const char* string, ...)
{
	va_list args;
	va_start(args, string);
	vprintf(string, args);
	va_end(args);
}
