#include "pch.h"
#include "CppUnitTest.h"
#include "..\\OddEngine\dllmain.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace OddEngineTests
{
    TEST_CLASS(OddEngineTests)
    {
    public:
        TEST_METHOD(InitGameEngine)
        {
			std::unique_ptr<GameEngine> TestEngine { new GameEngine(L"TestAssets\Gameobject.json") };
			Assert::IsTrue(true);
            // TODO: Your test code here
        }
    };
}