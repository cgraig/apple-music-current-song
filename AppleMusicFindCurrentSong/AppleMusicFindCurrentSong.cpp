#include <iostream>
#include <conio.h>
#include "AppleMusicPlayer.h"

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
            std::wcout << "Current Artist: " << amp.GetCurrentArtist() << std::endl;
            std::wcout << "Album: " << amp.GetCurrentAlbum() << std::endl;
            std::wcout << "Song: " << amp.GetCurrentSongTitle() << std::endl << std::endl;
        }

        Sleep(1000);
    } while (!_kbhit());

    return 0;
}