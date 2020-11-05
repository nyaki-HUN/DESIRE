#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

struct LogData;

// --------------------------------------------------------------------------------------------------------------------
//	Default filter policies
// --------------------------------------------------------------------------------------------------------------------

class NoFilterPolicy
{
public:
	inline bool Filter(const LogData& logData)
	{
		DESIRE_UNUSED(logData);
		return true;
	}
};

template<class Policy1, class Policy2>
class CombinedFilterPolicy
{
public:
	inline bool Filter(const LogData& logData)
	{
		return (m_policy1.Filter(logData) && m_policy2.Filter(logData));
	}

private:
	Policy1 m_policy1;
	Policy2 m_policy2;
};

// --------------------------------------------------------------------------------------------------------------------
//	Default output policies
// --------------------------------------------------------------------------------------------------------------------

class ConsoleOutputPolicy
{
public:
	void Process(const LogData& logData);
};

class FileOutputPolicy
{
public:
	FileOutputPolicy();
	void Process(const LogData& logData);

private:
	WriteFilePtr m_spLogFile;
};

// --------------------------------------------------------------------------------------------------------------------
//	Platform specific output policies
// --------------------------------------------------------------------------------------------------------------------

#if DESIRE_PLATFORM_WINDOWS

class ConsoleWindowOutputPolicy
{
public:
	ConsoleWindowOutputPolicy();
	void Process(const LogData& logData);
};

class VisualStudioOutputPolicy
{
public:
	void Process(const LogData& logData);
};

#endif
