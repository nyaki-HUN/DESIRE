#pragma once

#include "Engine/Core/Log.h"

struct NSLogOutputPolicy
{
	void Process(const Log::LogData& logData);
};
