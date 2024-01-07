#include <stdexcept>
#include <fstream>
#include "AppleMusicPlayer.h"

const std::string AppleMusicPlayer::APPLE_MUSIC_NAME = "Music";
const std::string AppleMusicPlayer::APPLE_SCRIPT_OUTPUT = "osascript_output.txt";
const std::string AppleMusicPlayer::APPLE_SCRIPT_CMD = "osascript "
    " -e \"try\""
    " -e \'tell application \"" + APPLE_MUSIC_NAME + "\"'"
    " -e \"set trackData to (get properties of current track)\""
    " -e \"set artistVar to trackData's artist\"" 
    " -e \"set albumVar to trackData's album\""
    " -e \"set songVar to trackData's name\""
    " -e \"log (artistVar)\""
    " -e \"log (albumVar)\""
    " -e \"log (songVar)\""
    " -e \"end tell\""
    " -e \"on error errStr number errorNumber\""
    " -e \"return -1\""
    " -e \"end try\""
    " > " + APPLE_SCRIPT_OUTPUT + " 2>&1";

AppleMusicPlayer::AppleMusicPlayer() : _CurrentAlbum(L""), _CurrentArtist(L""), _CurrentSong(L"")
{
    _Exit = false;
    _RunThread = std::thread(&AppleMusicPlayer::RunThread, this);
}

AppleMusicPlayer::~AppleMusicPlayer()
{
    _Exit = true;
    if (_RunThread.joinable())
    {
        _RunThread.join();
    }
}

void AppleMusicPlayer::RunThread()
{
    while (!_Exit) {
        if (IsAppleMusicRunning()) {
            if (system(APPLE_SCRIPT_CMD.c_str()) == 0) {
                std::wifstream osascriptOutput(APPLE_SCRIPT_OUTPUT, std::ofstream::in);
                if (osascriptOutput.is_open()) {
                    std::wstring line;
                    std::getline(osascriptOutput, line);

                    std::unique_lock<std::mutex> lock(_InfoMutex);
                    if (line.compare(L"-1") == 0) {
                        _CurrentArtist = L"";
                        _CurrentAlbum = L"";
                        _CurrentSong = L"";
                    }
                    else {
                        bool foundAlbum = false;
                        _CurrentArtist = line;

                        while (std::getline(osascriptOutput, line)) {
                            if (!foundAlbum) {
                                foundAlbum = true;
                                _CurrentAlbum = line;
                                continue;
                            }

                            _CurrentSong = line;
                        }
                    }
                    lock.unlock();
                }

                osascriptOutput.close();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

std::wstring AppleMusicPlayer::GetCurrentArtist()
{
    std::wstring artist;
    std::unique_lock<std::mutex> lock(_InfoMutex);
    artist = _CurrentArtist;
    lock.unlock();
    return artist;
}

std::wstring AppleMusicPlayer::GetCurrentAlbum()
{
    std::wstring album;
    std::unique_lock<std::mutex> lock(_InfoMutex);
    album = _CurrentAlbum;
    lock.unlock();
    return album;
}

std::wstring AppleMusicPlayer::GetCurrentSongTitle()
{
    std::wstring song;
    std::unique_lock<std::mutex> lock(_InfoMutex);
    song = _CurrentSong;
    lock.unlock();
    return song;
}

bool AppleMusicPlayer::IsAppleMusicRunning()
{
    return system("pgrep -u \"$(whoami)\" Music > /dev/null") == 0;
}