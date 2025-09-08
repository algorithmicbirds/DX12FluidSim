#include "Window/KeyBoard.hpp"

bool KeyBoard::KeyIsPressed(UCHAR Keycode) const noexcept { return Keystates[Keycode]; }

std::optional<KeyBoard::Event> KeyBoard::ReadKey() noexcept
{
    if (Keybuffer.size() > 0u)
    {
        KeyBoard::Event event = Keybuffer.front();
        Keybuffer.pop();
        return event;
    }
    return {};
}

bool KeyBoard::KeyIsEmpty() const noexcept { return Keybuffer.empty(); }

void KeyBoard::ClearKey() noexcept { Keybuffer = std::queue<KeyBoard::Event>(); }

std::optional<char> KeyBoard::ReadChar() noexcept
{
    if (Charbuffer.size() > 0u)
    {
        UCHAR charcode = Charbuffer.front();
        Charbuffer.pop();
        return charcode;
    }
    return {};
}

bool KeyBoard::CharIsEmpty() const noexcept { return Charbuffer.empty(); }

void KeyBoard::ClearChar() noexcept { Charbuffer = std::queue<char>(); }

void KeyBoard::Clear() noexcept
{
    ClearKey();
    ClearChar();
}

void KeyBoard::EnableAutorepeat() noexcept { AutorepeatEnabled = true; }

void KeyBoard::DisableAutorepeat() noexcept { AutorepeatEnabled = false; }

bool KeyBoard::AutorepeatIsEnabled() const noexcept { return AutorepeatEnabled; }

void KeyBoard::OnKeyPressed(UCHAR Keycode) noexcept
{
    Keystates[Keycode] = true;
    Keybuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Pressed, Keycode));
    TrimBuffer(Keybuffer);
}

void KeyBoard::OnKeyReleased(UCHAR Keycode) noexcept
{
    Keystates[Keycode] = false;
    Keybuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Released, Keycode));
    TrimBuffer(Keybuffer);
}


void KeyBoard::OnKeyHeld(UCHAR Keycode) noexcept
{
    Keybuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Held, Keycode));
    TrimBuffer(Keybuffer);
}

void KeyBoard::OnChar(char Character) noexcept
{
    Charbuffer.push(Character);
    TrimBuffer(Charbuffer);
}

void KeyBoard::ClearState() noexcept { Keystates.reset(); }

