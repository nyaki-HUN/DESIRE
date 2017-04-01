#pragma once

#include <memory>

class IReadFile;
typedef std::unique_ptr<IReadFile> ReadFilePtr;

class IWriteFile;
typedef std::unique_ptr<IWriteFile> WriteFilePtr;
