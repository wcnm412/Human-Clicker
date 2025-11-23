#include "keymap.h"
#include <Windows.h>

std::map<std::string, int> KeyMap = {
    // Function keys
    {"f1", VK_F1}, {"f2", VK_F2}, {"f3", VK_F3}, {"f4", VK_F4},
    {"f5", VK_F5}, {"f6", VK_F6}, {"f7", VK_F7}, {"f8", VK_F8},
    {"f9", VK_F9}, {"f10", VK_F10}, {"f11", VK_F11}, {"f12", VK_F12},

    // Numbers
    {"0", '0'}, {"1", '1'}, {"2", '2'}, {"3", '3'}, {"4", '4'},
    {"5", '5'}, {"6", '6'}, {"7", '7'}, {"8", '8'}, {"9", '9'},

    // Letters
    {"a", 'A'}, {"b", 'B'}, {"c", 'C'}, {"d", 'D'}, {"e", 'E'},
    {"f", 'F'}, {"g", 'G'}, {"h", 'H'}, {"i", 'I'}, {"j", 'J'},
    {"k", 'K'}, {"l", 'L'}, {"m", 'M'}, {"n", 'N'}, {"o", 'O'},
    {"p", 'P'}, {"q", 'Q'}, {"r", 'R'}, {"s", 'S'}, {"t", 'T'},
    {"u", 'U'}, {"v", 'V'}, {"w", 'W'}, {"x", 'X'}, {"y", 'Y'},
    {"z", 'Z'},

    // Special keys
    {"space", VK_SPACE},
    {"enter", VK_RETURN},
    {"shift", VK_SHIFT},
    {"ctrl", VK_CONTROL},
    {"alt", VK_MENU},
    {"tab", VK_TAB},
    {"esc", VK_ESCAPE},
    {"backspace", VK_BACK},
    {"delete", VK_DELETE},
    {"insert", VK_INSERT},
    {"home", VK_HOME},
    {"end", VK_END},
    {"pageup", VK_PRIOR},
    {"pagedown", VK_NEXT},

    // Arrow keys
    {"up", VK_UP},
    {"down", VK_DOWN},
    {"left", VK_LEFT},
    {"right", VK_RIGHT},

    // Numpad
    {"numpad0", VK_NUMPAD0}, {"numpad1", VK_NUMPAD1},
    {"numpad2", VK_NUMPAD2}, {"numpad3", VK_NUMPAD3},
    {"numpad4", VK_NUMPAD4}, {"numpad5", VK_NUMPAD5},
    {"numpad6", VK_NUMPAD6}, {"numpad7", VK_NUMPAD7},
    {"numpad8", VK_NUMPAD8}, {"numpad9", VK_NUMPAD9},

    // Mouse
    {"mouseleft", VK_LBUTTON},
    {"mouseright", VK_RBUTTON},
    {"mousemiddle", VK_MBUTTON},
    {"mouse4", VK_XBUTTON1},
    {"mouse5", VK_XBUTTON2},
};