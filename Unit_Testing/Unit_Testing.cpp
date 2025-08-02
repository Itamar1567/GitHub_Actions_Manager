#include "pch.h"
#include "CppUnitTest.h"
#include "../TaskTracker_GitHub/CurlSetup.h"
#include "../TaskTracker_GitHub/Validate.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(UnitTesting)
	{
	public:
		
		TEST_METHOD(IsGivenStringAnInteger)
		{
			std::string input = "123";
			Assert::IsTrue(Validate::IsInteger(input));
		}
	};
}
