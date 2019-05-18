#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

class String;

class IFileSource
{
protected:
	IFileSource() {}

public:
	virtual ~IFileSource() {}

	virtual ReadFilePtr OpenFile(const String& filename) = 0;
};
