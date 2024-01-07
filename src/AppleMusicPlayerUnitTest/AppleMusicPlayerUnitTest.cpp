#include "CppUnitTest.h"
#include <AppleMusicPlayer.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AppleMusicPlayerUnitTest
{
	// Unfortunately assumes Apple Music is already running
	// Also unfortunate is that it appears to COM appears to be
	// initialized by the test framework so any "no COM" testing
	// is moot with this framework.
	TEST_CLASS(AppleMusicPlayerUnitTest)
	{
	public:

		TEST_METHOD(InitDeinitAppleMusicPlayerHeap)
		{
			AppleMusicPlayer* amp = new AppleMusicPlayer();
			delete amp;
		}

		TEST_METHOD(InitDeinitAppleMusicPlayerOnStack)
		{
			AppleMusicPlayer amp;
		}

		TEST_METHOD(AppleMusicPlayerNoComIsRunning)
		{
			AppleMusicPlayer amp;
			Assert::IsTrue(amp.IsAppleMusicRunning());
		}

		TEST_METHOD(AppleMusicPlayerWithComIsRunning)
		{
			AppleMusicPlayer amp;
			Assert::IsTrue(amp.IsAppleMusicRunning());
		}

#ifdef FRAMEWORK_NOT_INIT_COM
		TEST_METHOD(AppleMusicPlayerNoComNoArtist)
		{
			AppleMusicPlayer amp;
			Assert::AreEqual(amp.GetCurrentArtist().compare(L""), 0);
		}

		TEST_METHOD(AppleMusicPlayerNoComNoAlbum)
		{
			AppleMusicPlayer amp;
			Assert::AreEqual(amp.GetCurrentAlbum().compare(L""), 0);
		}

		TEST_METHOD(AppleMusicPlayerNoComNoSong)
		{
			AppleMusicPlayer amp;
			Assert::AreEqual(amp.GetCurrentSongTitle().compare(L""), 0);
		}
#endif

		TEST_METHOD(AppleMusicPlayerWithComValidArtist)
		{
			AppleMusicPlayer amp;
			Assert::AreNotEqual(amp.GetCurrentArtist().compare(L""), 0);
		}

		TEST_METHOD(AppleMusicPlayerWithComValidAlbum)
		{
			AppleMusicPlayer amp;
			Assert::AreNotEqual(amp.GetCurrentAlbum().compare(L""), 0);
		}

		TEST_METHOD(AppleMusicPlayerWithComValidSong)
		{
			AppleMusicPlayer amp;
			Assert::AreNotEqual(amp.GetCurrentSongTitle().compare(L""), 0);
		}
	};
}
