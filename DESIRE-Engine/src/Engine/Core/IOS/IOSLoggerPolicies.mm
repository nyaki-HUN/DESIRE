#include "stdafx.h"
#include "Engine/Core/IOS/IOSLoggerPolicies.h"

#import <Foundation/Foundation.h>

void NSLogOutputPolicy::Process(const Log::LogData& logData)
{
	NSLog(@"[%s] %s\n", logData.logType, logData.message);
}

void XcodeOutputPolicy::Process(const Log::LogData& logData)
{
	// Xcode is using the "file:line: message" format for clickable messages
	NSLog(@"%s:%d: [%s] %s\n", logData.file, logData.line, logData.logType, logData.message);
}
