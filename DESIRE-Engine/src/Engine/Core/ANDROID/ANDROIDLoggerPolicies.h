#pragma once

#include "Engine/Core/Log.h"

struct LogCatOutputPolicy
{
	void Process(const Log::LogData& logData);
};
