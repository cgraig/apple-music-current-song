#pragma once

#ifdef WIN32
#define DllExport  __declspec( dllexport )
#else
#define DllExport
#endif

#include <string>
#ifdef WIN32
#include <uiautomation.h>
#include <atlbase.h>
#endif

class AppleMusicPlayer {
public:
    DllExport AppleMusicPlayer();

    // Check if Apple Music process is running
    DllExport bool IsAppleMusicRunning();

    // Get the artist of the current playing song
    DllExport std::wstring GetCurrentArtist();

    // Get the title of the current playing song
    DllExport std::wstring GetCurrentSongTitle();

    // Get the album of the current playing song
    DllExport std::wstring GetCurrentAlbum();

private:
#ifdef WIN32
    CComPtr<IUIAutomation> _pAutomation;
    CComPtr<IUIAutomationElement> _pSongElement;
    CComPtr<IUIAutomationElement> _pArtistAlbumElement;
    HWND _appleMusicHwnd;
    DWORD _appleMusicPid;
    bool IsAppleMusicPid(DWORD processId);
    bool IsAppleMusicStillRunning() { return IsAppleMusicPid(_appleMusicPid) && _appleMusicHwnd != nullptr; }
    HRESULT InitializeAutomation();
    static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
#endif

    bool FindAppleMusic();

    static const std::wstring APPLE_MUSIC_NAME;
};
