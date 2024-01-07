#include <stdexcept>
#include "AppleMusicPlayer.h"

const std::wstring AppleMusicPlayer::APPLE_MUSIC_NAME = L"AppleMusic.exe";

AppleMusicPlayer::AppleMusicPlayer()
{
}

static inline void ltrim(std::wstring& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

static inline void rtrim(std::wstring& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

static inline void trim(std::wstring& s) {
    rtrim(s);
    ltrim(s);
}

std::wstring AppleMusicPlayer::GetCurrentArtist()
{
    return L"";
}

std::wstring AppleMusicPlayer::GetCurrentAlbum()
{
    return L"";
}

std::wstring AppleMusicPlayer::GetCurrentSongTitle()
{
    return L"";
}

bool AppleMusicPlayer::FindAppleMusic()
{
    return false;
}

bool AppleMusicPlayer::IsAppleMusicRunning()
{
    return false;
}