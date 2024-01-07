#include <Windows.h>
#include <Psapi.h>
#include <stdexcept>
#include "AppleMusicPlayer.h"

const std::string AppleMusicPlayer::APPLE_MUSIC_NAME = "AppleMusic.exe";

AppleMusicPlayer::AppleMusicPlayer() : 
	_pAutomation(nullptr), _appleMusicHwnd(nullptr), _appleMusicPid(0), _pArtistAlbumElement(nullptr), _pSongElement(nullptr)
{
    if (FindAppleMusic()) {
        InitializeAutomation();
    }
}

bool AppleMusicPlayer::IsAppleMusicPid(DWORD processId)
{
    CHAR szProcessName[MAX_PATH] = "";
    bool found = false;

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE, 
        processId);
    if (NULL != hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleBaseNameA(hProcess, hMod, szProcessName,
                sizeof(szProcessName) / sizeof(CHAR));

            std::string processName(szProcessName);
            std::string appleMusicName = APPLE_MUSIC_NAME;
            if (processName.find(appleMusicName) != std::string::npos) {
                found = true;
            }
        }

        CloseHandle(hProcess);
    }

    return found;
}

BOOL CALLBACK AppleMusicPlayer::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    AppleMusicPlayer* amp = reinterpret_cast<AppleMusicPlayer*>(lParam);
    
    if (!amp) return FALSE; // don't even bother enumerating more windows

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);

    if (amp->_appleMusicPid == processId) {
        if (IsWindowVisible(hWnd) && GetWindowTextLength(hWnd) > 0) {
            amp->_appleMusicHwnd = hWnd;
            // Found it stop enumerating
            return FALSE;
        }
    }

    return TRUE; // Keep looking
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
    if (!IsAppleMusicStillRunning() || !_pArtistAlbumElement) {
        if (!FindAppleMusic()) {
            return L"";
        }

        if (FAILED(InitializeAutomation())) {
            return L"";
        }

        if (!_pArtistAlbumElement) {
            return L"";
        }
    }

    CComBSTR name;
    if (SUCCEEDED(_pArtistAlbumElement->get_CurrentName(&name))) {
        std::wstring wstr(static_cast<wchar_t*>(name));
        size_t foundPos = wstr.find(L'\u2014');
        if (foundPos != std::wstring::npos) {
            // Artist is beginning part of string preceding the \u2014 character
            std::wstring artist = wstr.substr(0, foundPos);

            trim(artist);

            return artist;
        }
    }

    return L"";
}

std::wstring AppleMusicPlayer::GetCurrentAlbum()
{
    if (!IsAppleMusicStillRunning() || !_pArtistAlbumElement) {
        if (!FindAppleMusic()) {
            return L"";
        }

        if (FAILED(InitializeAutomation())) {
            return L"";
        }

        if (!_pArtistAlbumElement) {
            return L"";
        }
    }

    CComBSTR name;
    if (SUCCEEDED(_pArtistAlbumElement->get_CurrentName(&name))) {
        std::wstring wstr(static_cast<wchar_t*>(name));
        size_t foundPos = wstr.find(L'\u2014');
        if (foundPos != std::wstring::npos) {
            // Album is second part of string after the \u2014 character
            std::wstring album = wstr.substr(foundPos + 1);

            // There could be multiple parts with another \u2014 character
            // grab album from in-between these 2 characters
            size_t nextEmPos = album.find(L'\u2014');
            if (nextEmPos != std::wstring::npos) {
                album = album.substr(0, nextEmPos);
            }

            trim(album);

            return album;
        }
    }

    return L"";
}

std::wstring AppleMusicPlayer::GetCurrentSongTitle()
{
    if (!IsAppleMusicStillRunning() || !_pSongElement) {
        if (!FindAppleMusic()) {
            return L"";
        }

        if (FAILED(InitializeAutomation())) {
            return L"";
        }

        if (!_pSongElement) {
            return L"";
        }
    }

    CComBSTR name;
    if (SUCCEEDED(_pSongElement->get_CurrentName(&name))) {
        std::wstring song(static_cast<wchar_t*>(name));
        return song;
    }

    return L"";
}

bool AppleMusicPlayer::FindAppleMusic()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    bool found = false;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return false;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    _appleMusicPid = 0;
    for (i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            if (IsAppleMusicPid(aProcesses[i])) {
                _appleMusicPid = aProcesses[i];
                found = true;
            }
        }
    }

    if (found) {
        _appleMusicHwnd = NULL;
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
    }

    return found && _appleMusicHwnd != nullptr;
}

HRESULT AppleMusicPlayer::InitializeAutomation()
{
    HRESULT hr = S_OK;

    if (_pAutomation == nullptr) {
        hr = CoCreateInstance(CLSID_CUIAutomation, nullptr,
            CLSCTX_INPROC_SERVER, IID_IUIAutomation,
            reinterpret_cast<void**>(&_pAutomation));
        if (FAILED(hr)) {
            _pAutomation = nullptr;
            return hr;
        }
    }
    
    CComPtr<IUIAutomationElement> root;
    hr = _pAutomation->ElementFromHandle(_appleMusicHwnd, &root);
    if (FAILED(hr) || root == nullptr) {
        if (SUCCEEDED(hr) && root == nullptr) {
            hr = E_FAIL;
            return hr;
        }
    }
    
    CComPtr<IUIAutomationCondition> transportBarCondition;
    if (FAILED(_pAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, CComVariant("TransportBar"), &transportBarCondition))) {
        return hr;
    }

    CComPtr<IUIAutomationElement> transportBar;
    hr = root->FindFirst(TreeScope_Descendants, transportBarCondition, &transportBar);
    if (FAILED(hr) || transportBar == nullptr) {
        if (SUCCEEDED(hr) && transportBar == nullptr) {
            hr = E_FAIL;
        }
        return hr;
    }

    CComPtr<IUIAutomationCondition> lcdCondition;
    if (FAILED(_pAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, CComVariant("LCD"), &lcdCondition))) {
        return hr;
    }

    CComPtr<IUIAutomationElement> lcd;
    hr = root->FindFirst(TreeScope_Descendants, lcdCondition, &lcd);
    if (FAILED(hr) || lcd == nullptr) {
        if (SUCCEEDED(hr) && lcd == nullptr) {
            hr = E_FAIL;
        }
        return hr;
    }

    CComPtr<IUIAutomationCondition> scrollingTextCondition;
    if (FAILED(_pAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, CComVariant("ScrollingText"), &scrollingTextCondition))) {
        return hr;
    }

    CComPtr<IUIAutomationElementArray> arr;
    if (FAILED(lcd->FindAll(TreeScope_Descendants, scrollingTextCondition, &arr))) {
        return hr;
    }

    int count = 0;
    arr->get_Length(&count);
    for (int i = 0; i < count; i++) {
        CComBSTR name;
        IUIAutomationElement *textElement;
        if (SUCCEEDED(arr->GetElement(i, &textElement))) {
            if (SUCCEEDED(textElement->get_CurrentName(&name))) {
                std::wstring wstr(static_cast<wchar_t*>(name));
                size_t foundPos = wstr.find(L'\u2014');
                if (foundPos != std::wstring::npos) {
                    _pArtistAlbumElement = textElement;
                }
                else {
                    _pSongElement = textElement;
                }
            }
        }
    }

    return hr;
}

bool AppleMusicPlayer::IsAppleMusicRunning()
{
    // Check if existing pid is still apple music
    if (IsAppleMusicStillRunning()) {
        return true;
    }

    return FindAppleMusic();
}