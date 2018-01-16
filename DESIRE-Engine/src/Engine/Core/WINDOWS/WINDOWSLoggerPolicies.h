#pragma once

#include "Core/Log.h"

struct WIN32ConsoleOutputPolicy
{
	WIN32ConsoleOutputPolicy();
	void Process(const Log::LogData& logData);
};

struct VisualStudioOutputPolicy
{
	void Process(const Log::LogData& logData);
};
