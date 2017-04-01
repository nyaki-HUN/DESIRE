#pragma once

#include "Core/Log.h"

struct NSLogOutputPolicy
{
	void Process(const Log::SLogData& logData);
};
