#include "Engine/stdafx.h"
#include "Engine/Core/Log/LoggerPolicies.h"

#if DESIRE_PLATFORM_IOS

#import <Foundation/Foundation.h>

void NSLogOutputPolicy::Process(const LogData& logData)
{
	NSLog(@"[%s] %s\n", logData.logType, logData.message);
}

void XcodeOutputPolicy::Process(const LogData& logData)
{
	// Xcode is using the "file:line: message" format for clickable messages
	NSLog(@"%s:%d: [%s] %s\n", logData.file, logData.line, logData.logType, logData.message);
}

#endif	// #if DESIRE_PLATFORM_IOS
