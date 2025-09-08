#pragma once 

#include "GlobInclude/WinInclude.hpp"
#include <optional>
#include <queue>
#include <bitset>

class KeyBoard
{
    friend class Window;

public:
    KeyBoard() = default;
    KeyBoard(const KeyBoard &) = delete;
    KeyBoard &operator=(const KeyBoard &) = delete;

public:
    class Event
    {
    public:
        enum class Type
        {
            Pressed,
            Held,
            Released
        };

    private:
        Type m_Type;
        UCHAR m_Code;

    public:
        Event(Type Type, UCHAR Code) noexcept : m_Type(Type), m_Code(Code) {}
        bool IsPress() const noexcept { return m_Type == Type::Pressed; }
        bool IsRelease() const noexcept { return m_Type == Type::Released; }
        bool IsHeld() const noexcept { return m_Type == Type::Held; }
        unsigned char GetCode() const noexcept { return m_Code; }
    };

public:
    bool KeyIsPressed(UCHAR Keycode) const noexcept;
    std::optional<Event> ReadKey() noexcept;
    bool KeyIsEmpty() const noexcept;
    void ClearKey() noexcept;

    std::optional<char> ReadChar() noexcept;
    bool CharIsEmpty() const noexcept;
    void ClearChar() noexcept;
    void Clear() noexcept;

    void EnableAutorepeat() noexcept;
    void DisableAutorepeat() noexcept;
    bool AutorepeatIsEnabled() const noexcept;

private:
    void OnKeyPressed(UCHAR Keycode) noexcept;
    void OnKeyReleased(UCHAR Keycode) noexcept;
    void OnKeyHeld(UCHAR Keycode) noexcept;
    void OnChar(char Character) noexcept;
    void ClearState() noexcept;
    template <typename T> static void TrimBuffer(std::queue<T> &Buffer) noexcept;

private:
    static constexpr UINT NKeys = 256u;
    static constexpr UINT BufferSize = 16u;
    bool AutorepeatEnabled = true;
    std::bitset<NKeys> Keystates;
    std::queue<Event> Keybuffer;
    std::queue<char> Charbuffer;

};

template <typename T> inline void KeyBoard::TrimBuffer(std::queue<T> &Buffer) noexcept
{
    while (Buffer.size() > BufferSize)
    {
        Buffer.pop();
    }
}