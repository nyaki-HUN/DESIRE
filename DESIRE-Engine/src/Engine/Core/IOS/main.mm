#include "stdafx.h"

#import <UIKit/UIKit.h>

#if defined(DESIRE_DISTRIBUTION)
	#include <unistd.h>
	#include <sys/syscall.h>
	#include <sys/types.h>
	#include <sys/sysctl.h>
#endif

int main(int argc, char *argv[])
{
#if defined(DESIRE_DISTRIBUTION)
	// Anti-hacking code to prevent the debugger from attaching our process
	syscall(SYS_ptrace, 31/*PT_DENY_ATTACH*/, 0, 0, 0);

	int name[] =
	{
		CTL_KERN,
		KERN_PROC,
		KERN_PROC_PID,
		getpid()
	}
	
	kinfo_proc info;
	memset(&info, 0, sizeof(kinfo_proc));
	size_t size = sizeof(kinfo_proc);

	if(sysctl(name, DESIRE_ASIZEOF(name), &info, &size, nullptr, 0) != 0)
	{
		int *ptr = nullptr;
		*ptr = 0;
	}

	if(info.kp_proc.p_flag & P_TRACED)
	{
		// Code to react to debugging goes here
		int *ptr = nullptr;
		*ptr = 0;
	}
#endif

	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	int result = UIApplicationMain(argc, argv, @"AppDelegate", @"AppDelegate");
	[pool release];

	return result;
}
