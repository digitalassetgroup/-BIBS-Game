#pragma once
#include <platform/CCPlatformConfig.h>
#include "Core/CoreAll.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	#include "../../win32/WindowsStackWalker.h"
	#include <stdio.h>

	// Simple implementation of an additional output to the console:
	class MyStackWalker : NonCopyable, protected StackWalker
	{
		private_ int _maxCountOfFrames;
		private_ int _currentFrame;

		private_ MyStackWalker(int maxCountOfFrames) 
			: StackWalker(), _maxCountOfFrames(maxCountOfFrames), _currentFrame() {}
		private_ virtual void OnOutput(LPCSTR szText) 
		{ 
			++_currentFrame;
			StackWalker::OnOutput(szText);
		}

		virtual bool IsToFinishOutput()
		{
			return (_currentFrame >= _maxCountOfFrames);
		}

		public_ static MyStackWalker& Inst()
		{
			static MyStackWalker walker(5);
			return walker;
		}

		public_ void PrintWholeStackTrace(int maxCountOfFrames)
		{
			_maxCountOfFrames = maxCountOfFrames;
			_currentFrame = 0;
			StackWalker::OnOutput("------BEGIN STACKTRACE------\n");
			StackWalker::ShowCallstack();
			StackWalker::OnOutput("------END STACKTRACE------\n");
		}
	};	

	#define PRINT_STACK_TRACE(maxCountOfFrames) \
		do { MyStackWalker::Inst().PrintWholeStackTrace(maxCountOfFrames); } while(NULL)

	//#define PRINT_STACK_TRACE() \
	//	do { MyStackWalker sw; sw.ShowCallstack(); } while(NULL)
#else
	#define PRINT_STACK_TRACE() (void)0
#endif
