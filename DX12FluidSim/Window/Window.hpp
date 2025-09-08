#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Window/KeyBoard.hpp"
#include "Window/Mouse.hpp"

class Window
{
public:
    Window();
    ~Window();
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    void UpdateMsg();

    bool ShouldClose() const { return bShouldClose; }
    bool ShouldResize() const { return bShouldResize; }
    void ClearResizeFlags() { bShouldResize = false; }

    HWND GetHwnd() const { return Hwnd; }
    void SetFullScreen(bool EnableFullScreen);
    void FullScreenFlipFlop();
    float GetDeltaTimeSeconds();
    void UpdateKeyBoard();

    bool KeyPressed(UCHAR Keycode) { return KeyDown[Keycode]; }
    bool KeyReleased(UCHAR KeyCode) { return KeyUp[KeyCode]; };
    void ResetKeyBoardState() {
        KeyDown.reset();
        KeyUp.reset();
    };

public:
    KeyBoard Keyboard;
    Mouse Mouse;

private:
    bool Init();
    void ShutDown();
    static LRESULT CALLBACK StaticWindowProc(HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam);
    LRESULT WindowProc(HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam);

private:
    HWND Hwnd = nullptr;
    ATOM WindowClass = 0;
    RECT SavedWindowRect{};

    static inline bool bShouldClose = false;
    static inline bool bShouldResize = false;
    static inline bool bIsFullScreenEnabled = false;

    DWORD DefaultStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
    DWORD EXDefaultStyle = WS_EX_APPWINDOW;
    LARGE_INTEGER Frequency{};
    LARGE_INTEGER LastTime{};

    std::bitset<256> KeysState{}; 
    std::bitset<256> KeyDown{};
    std::bitset<256> KeyUp{};

};
