#pragma once

#include "../Base.h"

// Windows平台DLL导出/导入宏定义
#ifdef TC_PLATFORM_WINDOWS
	#ifdef TC_BUILD_DLL
		#define TERMINALCORE_API __declspec(dllexport)
	#else
		#define TERMINALCORE_API __declspec(dllimport)
	#endif
#else
	#error TerminalCore only support Windows!
#endif