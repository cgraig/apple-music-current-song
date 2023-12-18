#include <iostream>
#include <conio.h>
#include <algorithm>
#include "AppleMusicPlayer.h"

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
    AppleMusicPlayer amp;
    bool found = false;

    do
    {
        std::cout << "Trying to find Apple Music process...";

        if (found = amp.IsAppleMusicRunning()) {
            break;
        }

        std::cout << "Not Found!" << std::endl;
        std::cout << "Press any key to exit." << std::endl;
        Sleep(500);

    } while (!_kbhit());

    if (!found)
    {
        return 1;
    }

    std::cout << "Found!" << std::endl;

    std::cout << "Press any key to exit" << std::endl;

    std::wstring previousSong = L"UNKNOWN";
    std::wstring currentArtist;
    std::wstring currentAlbum;
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

    return 0;
}