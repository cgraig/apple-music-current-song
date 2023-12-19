#pragma once

#include <string>
#include <uiautomation.h>
#include <atlbase.h>

class AppleMusicPlayer {
public:
    // Default constructor will initialize COM
    AppleMusicPlayer();
    // Provide another constructor to explicitly opt in/out of
    // COM init/deinit
    AppleMusicPlayer(bool initializeCom);

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
    // Do not use CComPtr for these objects as it's possible
    // CoUninitialize could get called in destructor which
    // would happen before an object of this class gets fully
    // de-initialized.  So ->Release would get called after
    // CoUnitialize resulting in crashes...
    // Furthermore we want to keep these objects in scope
    // for lifetime of object any way, just need to remember
    // to release these at the appropriate times.
    IUIAutomation *_pAutomation;
    IUIAutomationElement *_pSongElement;
    IUIAutomationElement *_pArtistAlbumElement;
    HWND _appleMusicHwnd;
    DWORD _appleMusicPid;
    bool _comInitializedByUser;

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
