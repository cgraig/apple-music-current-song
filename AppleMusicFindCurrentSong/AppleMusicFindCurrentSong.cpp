#include <iostream>
#include <conio.h>
#include <algorithm>
#include <AppleMusicPlayer.h>

std::string ConvertToAscii(std::wstring inputStr)
{
    std::string str;
    std::transform(inputStr.begin(), inputStr.end(), std::back_inserter(str), [](wchar_t c) {
        return (char)c;
        });
    return str;
}

int main()
{
    bool appleMusicFound = false;

    // First we must initialize COM as the AppleMusicPlayer object/class requires it
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        std::cerr << "Failed to initialize COM, exiting. (Error Code: " << hr << ")" << std::endl;
        return hr;
    }

    //
    // Two options here with COM in play...
    // 1. Put AppleMusicPlayer on the heap and make sure to delete it
    // before calling ComUninitialize
    // OR
    // 2. Put AppleMusicPlayer in its own scope { } as a "child" of whatever
    // will call CoUninitialize.
    // 
    // Opting for #2
    //
    {
        AppleMusicPlayer amp;

        std::wstring previousSong = L"UNKNOWN";
        std::wstring currentArtist;
        std::wstring currentAlbum;
        

        do
        {
            std::cout << "Trying to find Apple Music process...";

            if (appleMusicFound = amp.IsAppleMusicRunning()) {
                break;
            }

            std::cout << "Not Found!" << std::endl;
            std::cout << "Press any key to exit." << std::endl;
            Sleep(500);

        } while (!_kbhit());

        if (!appleMusicFound)
        {
            goto Exit;
        }

        std::cout << "Found!" << std::endl;

        std::cout << "Press any key to exit" << std::endl;

        do
        {
            bool update = false;

            // Check song, only update if the song title changes
            std::wstring currentSong = amp.GetCurrentSongTitle();
            if (previousSong.compare(currentSong) != 0)
            {
                update = true;
                previousSong = currentSong;
            }

            if (update) {
                // The console might not like unicode characters unfortunately,
                // convert them to ASCII before printing out
                std::cout << "Current Artist: " << ConvertToAscii(amp.GetCurrentArtist()) << std::endl;
                std::cout << "Album: " << ConvertToAscii(amp.GetCurrentAlbum()) << std::endl;
                std::cout << "Song: " << ConvertToAscii(currentSong) << std::endl << std::endl;
            }

            Sleep(1000);
        } while (!_kbhit());
    }

Exit:
    // Don't forget to uninitialize COM
    CoUninitialize();

    if (!appleMusicFound) {
        return 1;
    }

    return 0;
}