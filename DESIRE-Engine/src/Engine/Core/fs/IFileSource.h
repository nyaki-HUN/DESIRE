#pragma once

#include "Engine/Core/fs/FilePtr_fwd.h"

class String;

class IFileSource
{
protected:
	IFileSource() {}

public:
	virtual ~IFileSource() {}

	virtual ReadFilePtr OpenFile(const String& filename) = 0;
};
