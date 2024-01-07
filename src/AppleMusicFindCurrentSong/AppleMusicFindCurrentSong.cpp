#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <thread>
#include <time.h>
#include <AppleMusicPlayer.h>

#ifndef WIN32
#include <sys/syslimits.h>
#endif

std::string ConvertToAscii(std::wstring inputStr)
{
    std::string str;
    std::transform(inputStr.begin(), inputStr.end(), std::back_inserter(str), [](wchar_t c) {
        return (char)c;
        });
    return str;
}

bool g_OutputSong = false;
bool g_SplitFiles = false;
bool g_WriteLabels = false;
bool g_WriteTrackList = false;
bool g_DisplayHelp = false;
std::string g_ArtistFileName = "artist.txt";
std::string g_AlbumFileName = "album.txt";
std::string g_SongFileName = "song.txt";
std::string g_FullSongInfoFileName = "fullsonginfo.txt";
std::filesystem::path g_OutputFilePath = ".";
std::atomic<bool> g_ExitFlag(false);

void Usage()
{
    std::cerr << "AppleMusicFindCurrentSong: Will detect current playing song in Apple Music Preview app on Windows.";
    std::cerr << "  It will by default print out song it detects and also write to a file.  File information can be overriden with";
    std::cerr << " various command line options." << std::endl << std::endl;
    std::cerr << "usage: AppleMusicFindCurrentSong [-o|--output-file] <file name> [-p|--path] <path> [-l|--labels] [-s|--split] [-t|--track-list] [-?|--help]" << std::endl;
    std::cerr << "\t-o|--output-file\tName of file to output full song information for, will default to fullsonginfo.txt." << std::endl;
    std::cerr << "\t-p|--path\tName of folder to output song information to, will default to current directory." << std::endl;
    std::cerr << "\t-l|--labels\tOption to add Artist:/Album:/Song: prefix to fullsonginfo.txt for each content." << std::endl;
    std::cerr << "\t-s|--split\tOption to split Artist/Album/Song info into their own files artist.txt, album.txt, and song.txt." << std::endl;
    std::cerr << "\t-t|--track-list\tOption to write Artist - Song for each song detected to a tracklist file." << std::endl;
    std::cerr << "\t-?|--help\tDisplays this information." << std::endl;
    std::cerr << std::endl;
}

void ParseCommandLine(const int argc, const char* argv[])
{
    std::vector<std::string_view> args(argv + 1, argv + argc);

    if (argc > 9) {
        throw std::runtime_error("Too many command line arguments.");
    }

    for (auto it = args.begin(), end = args.end(); it != end; ++it) {
        if (*it == "-o" || *it == "--output-file") {
            if (it + 1 != end) {
                it++;
                g_FullSongInfoFileName = *it;
            }
            else {
                throw std::runtime_error("You must specify a file name with the -o option.");
            }
        }
        else if (*it == "-p" || *it == "--path") {
            if (it + 1 != end) {
                it++;
                g_OutputFilePath = *it;
            }
            else {
                throw std::runtime_error("You must specify a path with the -p option.");
            }
        }
        else if (*it == "-l" || *it == "--labels") {
            g_WriteLabels = true;
        }
        else if (*it == "-s" || *it == "--split") {
            g_SplitFiles = true;
        }
        else if (*it == "-t" || *it == "--track-list") {
            g_WriteTrackList = true;
        }
        else if (*it == "-?" || *it == "--help") {
            g_DisplayHelp = true;
        }
        else {
            throw std::runtime_error("Unknown option specified.");
        }
    }

    if (!std::filesystem::exists(g_OutputFilePath)) {
        throw std::runtime_error("Folder (" + g_OutputFilePath.string() + ") not found to write output files to.");
    }

    g_OutputSong = true;
}

void wait_for_exit_function() {
    std::cin.get();
    g_ExitFlag = true;
}

int main(const int argc, const char *argv[])
{
    bool appleMusicFound = false;
#ifdef WIN32
    const uint32_t max_path = MAX_PATH;
#else
    const uint32_t max_path = PATH_MAX;
#endif
    char trackListFileName[max_path];
    std::ofstream trackListOutputFile;

    try {
        ParseCommandLine(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "AppleMusicFindCurrentSong: " << e.what() << std::endl;
        Usage();
        return EXIT_FAILURE;
    }

    if (g_DisplayHelp) {
        Usage();
        return EXIT_SUCCESS;
    }

#ifdef WIN32
    // First we must initialize COM as the AppleMusicPlayer object/class requires it
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM, exiting. (Error Code: " << hr << ")" << std::endl;
        return hr;
    }
#endif

    std::thread t(wait_for_exit_function);

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
        
        std::string currentArtistAscii;
        std::string currentAlbumAscii;
        std::string currentSongAscii;

        std::cout << "Trying to find Apple Music process...";
        bool pressAnyKeyOutput = false;

		do {
			if ((appleMusicFound = amp.IsAppleMusicRunning())) {
				break;
			}

            if (!pressAnyKeyOutput) {
                std::cout << "Not Found!" << std::endl;
                std::cout << "Will wait until it launches, otherwise, press ENTER to exit." << std::endl;
                pressAnyKeyOutput = true;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));

		} while (!g_ExitFlag);

        if (!appleMusicFound) {
            goto Exit;
        }

        std::cout << "Found!" << std::endl;

        std::cout << "Press ENTER to exit" << std::endl << std::endl;

        if (g_WriteTrackList) {
            time_t timeNow = time(NULL);
            struct tm tmTimeNow;

#ifdef WIN32
            if (localtime_s(&tmTimeNow, &timeNow) != 0 || !strftime(trackListFileName, max_path, "tracklist_%Y-%m-%d_%H-%M-%S.txt", &tmTimeNow)) {
                std::cerr << "WARNING: could not create a local track list file name, will not write track listing." << std::endl;
                g_WriteTrackList = false;
            }
#else
            if (!localtime_r(&timeNow, &tmTimeNow) || !strftime(trackListFileName, max_path, "tracklist_%Y-%m-%d_%H-%M-%S.txt", &tmTimeNow)) {
                std::cerr << "WARNING: could not create a local track list file name, will not write track listing." << std::endl;
                g_WriteTrackList = false;
            }
#endif

            if (g_WriteTrackList) {
                std::filesystem::path trackListFullPath = g_OutputFilePath / trackListFileName;
                trackListOutputFile.open(trackListFullPath, std::ofstream::out | std::ofstream::app);
                if (!trackListOutputFile.is_open()) {
                    std::cerr << "WARNING! Failed to open " << trackListFullPath << " to write track list contents.  Will not write track listing." << std::endl;
                    g_WriteTrackList = false;
                }
            }
        }

        std::filesystem::path artistFilePath;
        std::filesystem::path albumFilePath;
        std::filesystem::path songFilePath;
        std::filesystem::path fullSongInfoFullPath;
        if (g_OutputSong) {
            fullSongInfoFullPath = g_OutputFilePath / g_FullSongInfoFileName;
        }

        if (g_SplitFiles) {
            // get the folder from g_OutputFilePath
            artistFilePath = g_OutputFilePath / g_ArtistFileName;
            albumFilePath = g_OutputFilePath / g_AlbumFileName;
            songFilePath = g_OutputFilePath / g_SongFileName;
        }

        do {
            bool update = false;

            // Check song, only update if the song title changes
            std::wstring currentSong = amp.GetCurrentSongTitle();
            if (previousSong.compare(currentSong) != 0) {
                update = true;
                previousSong = currentSong;
            }

            if (update) {
                currentArtistAscii = ConvertToAscii(amp.GetCurrentArtist());
                currentAlbumAscii = ConvertToAscii(amp.GetCurrentAlbum());
                currentSongAscii = ConvertToAscii(currentSong);

                // The console might not like unicode characters unfortunately,
                // convert them to ASCII before printing out
                std::cout << "Artist: " << currentArtistAscii << std::endl;
                std::cout << "Album: " << currentAlbumAscii << std::endl;
                std::cout << "Song: " << currentSongAscii << std::endl << std::endl;

                if (g_OutputSong) {
                    std::ofstream outputFile(fullSongInfoFullPath);
                    if (!outputFile.is_open()) {
                        std::cerr << "WARNING! Failed to open " << fullSongInfoFullPath << " to write contents." << std::endl;
                    }
                    else {
                        if (g_WriteLabels) {
                            outputFile << "Artist: " << currentArtistAscii << std::endl;
                            outputFile << "Album: " << currentAlbumAscii << std::endl;
                            outputFile << "Song: " << currentSongAscii << std::endl;
                        }
                        else {
                            outputFile << currentArtistAscii << std::endl;
                            outputFile << currentAlbumAscii << std::endl;
                            outputFile << currentSongAscii << std::endl;
                        }

                        outputFile.close();
                    }

                    if (g_SplitFiles) {
                        std::ofstream artistOutputFile(artistFilePath);
                        if (artistOutputFile.is_open()) {
                            artistOutputFile << currentArtistAscii;
                            artistOutputFile.close();
                        }
                        else {
                            std::cerr << "WARNING! Failed to open " << artistFilePath << " to write artist name." << std::endl;
                        }

                        std::ofstream albumOutputFile(albumFilePath);
                        if (albumOutputFile.is_open()) {
                            albumOutputFile << currentAlbumAscii;
                            albumOutputFile.close();
                        }
                        else {
                            std::cerr << "WARNING! Failed to open " << albumFilePath << " to write album name." << std::endl;
                        }

                        std::ofstream songOutputFile(songFilePath);
                        if (songOutputFile.is_open()) {
                            songOutputFile << currentSongAscii;
                            songOutputFile.close();
                        }
                        else {
                            std::cerr << "WARNING! Failed to open " << songFilePath << " to write song name." << std::endl;
                        }
                    }

                    if (g_WriteTrackList) {
                        trackListOutputFile << currentArtistAscii << " - " << currentSongAscii << std::endl;
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } while (!g_ExitFlag);
    }

Exit:
#ifdef WIN32
    // Don't forget to uninitialize COM
    CoUninitialize();
#endif

    if (g_WriteTrackList && trackListOutputFile.is_open()) {
        trackListOutputFile.close();
    }

    if (g_ExitFlag) {
        t.join();
    }

    if (!appleMusicFound) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
