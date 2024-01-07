#pragma once

#ifdef _WIN32
#define DllExport  __declspec( dllexport )
#else
#define DllExport
#endif

#include <string>
#ifdef _WIN32
#include <uiautomation.h>
#include <atlbase.h>
#endif
#ifdef __APPLE__
#include <thread>
#include <mutex>
#endif

class AppleMusicPlayer {
public:
    DllExport AppleMusicPlayer();

#ifdef __APPLE__
    DllExport ~AppleMusicPlayer();
#endif

    // Check if Apple Music process is running
    DllExport bool IsAppleMusicRunning();

    // Get the artist of the current playing song
    DllExport std::wstring GetCurrentArtist();

    // Get the title of the current playing song
    DllExport std::wstring GetCurrentSongTitle();

    // Get the album of the current playing song
    DllExport std::wstring GetCurrentAlbum();

private:
#ifdef _WIN32
    CComPtr<IUIAutomation> _pAutomation;
    CComPtr<IUIAutomationElement> _pSongElement;
    CComPtr<IUIAutomationElement> _pArtistAlbumElement;
    HWND _appleMusicHwnd;
    DWORD _appleMusicPid;
    bool IsAppleMusicPid(DWORD processId);
    bool IsAppleMusicStillRunning() { return IsAppleMusicPid(_appleMusicPid) && _appleMusicHwnd != nullptr; }
    HRESULT InitializeAutomation();
    static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
    bool FindAppleMusic();
#endif

    static const std::string APPLE_MUSIC_NAME;

#ifdef __APPLE__
    static const std::string APPLE_SCRIPT_CMD;
    static const std::string APPLE_SCRIPT_OUTPUT;
    std::thread _RunThread;
    void RunThread();
    std::mutex _InfoMutex;
    std::atomic<bool> _Exit;
    std::wstring _CurrentArtist;
    std::wstring _CurrentAlbum;
    std::wstring _CurrentSong;
#endif
};
