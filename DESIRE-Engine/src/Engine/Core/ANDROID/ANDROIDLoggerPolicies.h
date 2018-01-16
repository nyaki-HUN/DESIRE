#pragma once

#include "Core/Log.h"

struct LogCatOutputPolicy
{
	void Process(const Log::LogData& logData);
};
