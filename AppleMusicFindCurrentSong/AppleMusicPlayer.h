#pragma once

#include <string>
#include <uiautomation.h>
#include <atlbase.h>

class AppleMusicPlayer {
public:
    // Constructor (initialize any necessary resources)
    AppleMusicPlayer();

    // Destructor (clean up resources)
    ~AppleMusicPlayer();

    // Check if Apple Music process is running
    bool IsAppleMusicRunning();

    // Get the artist of the current playing song
    std::wstring GetCurrentArtist();

    // Get the title of the current playing song
    std::wstring GetCurrentSongTitle();

    // Get the album of the current playing song
    std::wstring GetCurrentAlbum();

private:
    IUIAutomation *_pAutomation;
    IUIAutomationElement *_pSongElement;
    IUIAutomationElement *_pArtistAlbumElement;
    HWND _appleMusicHwnd;
    DWORD _appleMusicPid;

    bool FindAppleMusic();
    bool IsAppleMusicPid(DWORD processId);
    bool IsAppleMusicStillRunning() { return IsAppleMusicPid(_appleMusicPid) && _appleMusicHwnd != nullptr; }
    HRESULT InitializeAutomation();
    static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

    inline static const std::wstring APPLE_MUSIC_NAME = L"AppleMusic.exe";
};

class ComException : public std::runtime_error {
public:
    ComException(std::string Message, HRESULT hr) : _hr(hr), std::runtime_error(Message) { }
    HRESULT GetErrorCode() { return _hr; }

private:
    HRESULT _hr;
};
