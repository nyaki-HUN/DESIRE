#include "stdafx.h"
#include "Core/fs/FileSystem.h"

#import <Foundation/Foundation.h>

FilePtr FileSystem::OpenNative(const char *filename, ELocation location, bool createForWriting)
{
	FILE *file = nullptr;

	// If the file mode is writeable, skip all the bundle stuff because generally the bundle is read-only
	if(strcmp("r", mode) && strcmp("rb", mode))
	{
		file = fopen(filename, mode);
		return file;
	}

	@autoreleasepool
	{
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSFileManager *nsFileManager = [NSFileManager defaultManager];
		NSString *strFilename = [nsFileManager stringWithFileSystemRepresentation:filename length:strlen(filename)];
		NSString *filenameWithPath = [resourcePath stringByAppendingPathComponent:strFilename];
		if([nsFileManager fileExistsAtPath:filenameWithPath])
		{
			file = fopen([filenameWithPath fileSystemRepresentation], mode);
		}
		else
		{
			file = fopen(filename, mode);
		}
	}

	return file;
}
