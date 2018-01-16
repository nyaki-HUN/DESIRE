#pragma once

#include "Engine/Core/fs/FilePtr_fwd.h"

class IFileSource
{
protected:
	IFileSource() {}

public:
	virtual ~IFileSource() {}

	virtual ReadFilePtr OpenFile(const char *filename) = 0;
};
