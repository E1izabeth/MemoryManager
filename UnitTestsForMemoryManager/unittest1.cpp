#include "stdafx.h"
#include "CppUnitTest.h"
#include "D:\MyProjects\MemoryManager\MemoryManager\mem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsForMemoryManager
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(TestMethod1)
		{
			wchar_t message[200];
			try
			{
				auto p1 = memAlloc(2048);
				auto p2 = memAlloc(4096);
				auto p3 = memAlloc(2048);
				memFree(p1);
				memFree(p2);
				memFree(p3);
			}
			catch(...)
			{
				Assert::Fail(message, LINE_INFO());
			}
		}

	};
}