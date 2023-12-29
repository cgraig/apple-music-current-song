#include <iostream>
#include <fstream>
#include <conio.h>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <AppleMusicPlayer.h>

std::string ConvertToAscii(std::wstring inputStr)
{
    std::string str;
    std::transform(inputStr.begin(), inputStr.end(), std::back_inserter(str), [](wchar_t c) {
        return (char)c;
        });
    return str;
}

bool g_OutputSong = false;
bool g_WriteLabels = false;
std::filesystem::path g_OutputFilePath = "";

void Usage()
{
    std::cerr << "usage: AppleMusicFindCurrentSong [-o|--output-file] <full_path> [-l|--labels]" << std::endl;
}

void ParseCommandLine(const int argc, const char* argv[])
{
    std::vector<std::string_view> args(argv + 1, argv + argc);
    std::filesystem::path fullFilePath = "";
    std::filesystem::path filePath = "";

    if (argc > 4) {
        throw std::runtime_error("Too many command line arguments.");
    }

    for (auto it = args.begin(), end = args.end(); it != end; ++it) {
        if (*it == "-o" || *it == "--output-file") {
            if (it + 1 != end) {
                it++;
                fullFilePath = *it;
                filePath = fullFilePath;
            }
            else {
                throw std::runtime_error("You must specify a full path with the -o option.");
            }
        }
        else if (*it == "-l" || *it == "--labels") {
            g_WriteLabels = true;
        }
        else {
            throw std::runtime_error("Unknown option specified.");
        }
    }

    // User didn't specify an output path which is fine as it's optional
    if (fullFilePath == "") {
        return;
    }

    if (!std::filesystem::exists(filePath.remove_filename())) {
        throw std::runtime_error("Folder (" + filePath.string() + ") not found to write output file to.");
    }

    g_OutputFilePath = fullFilePath;
    g_OutputSong = true;
}

int main(const int argc, const char *argv[])
{
    bool appleMusicFound = false;

    try {
        ParseCommandLine(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "AppleMusicFindCurrentSong: " << e.what() << std::endl;
        Usage();
        return EXIT_FAILURE;
    }

    // First we must initialize COM as the AppleMusicPlayer object/class requires it
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
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

        std::cout << "Trying to find Apple Music process...";
        bool pressAnyKeyOutput = false;
        do {
            if (appleMusicFound = amp.IsAppleMusicRunning()) {
                break;
            }

            if (!pressAnyKeyOutput) {
                std::cout << "Not Found!" << std::endl;
                std::cout << "Will wait until it launches, otherwise, press any key to exit." << std::endl;
                pressAnyKeyOutput = true;
            }

            Sleep(500);

        } while (!_kbhit());

        if (!appleMusicFound) {
            goto Exit;
        }

        std::cout << "Found!" << std::endl;

        std::cout << "Press any key to exit" << std::endl << std::endl;

        do {
            bool update = false;

            // Check song, only update if the song title changes
            std::wstring currentSong = amp.GetCurrentSongTitle();
            if (previousSong.compare(currentSong) != 0) {
                update = true;
                previousSong = currentSong;
            }

            if (update) {
                currentArtist = amp.GetCurrentArtist();
                currentAlbum = amp.GetCurrentAlbum();

                // The console might not like unicode characters unfortunately,
                // convert them to ASCII before printing out
                std::cout << "Artist: " << ConvertToAscii(currentArtist) << std::endl;
                std::cout << "Album: " << ConvertToAscii(currentAlbum) << std::endl;
                std::cout << "Song: " << ConvertToAscii(currentSong) << std::endl << std::endl;

                if (g_OutputSong) {
                    std::ofstream outputFile(g_OutputFilePath);
                    if (!outputFile.is_open()) {
                        std::cerr << "WARNING! Failed to open " << g_OutputFilePath << " to write contents." << std::endl;
                    }
                    else {
                        if (g_WriteLabels) {
                            outputFile << "Artist: " << ConvertToAscii(currentArtist) << std::endl;
                            outputFile << "Album: " << ConvertToAscii(currentAlbum) << std::endl;
                            outputFile << "Song: " << ConvertToAscii(currentSong) << std::endl;
                        }
                        else {
                            outputFile << ConvertToAscii(currentArtist) << std::endl;
                            outputFile << ConvertToAscii(currentAlbum) << std::endl;
                            outputFile << ConvertToAscii(currentSong) << std::endl;
                        }

                        outputFile.close();
                    }
                }
            }

            Sleep(1000);
        } while (!_kbhit());
    }

Exit:
    // Don't forget to uninitialize COM
    CoUninitialize();

    if (!appleMusicFound) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}