#include "main.h"

std::atomic<bool> Clicking = true;

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

void printUsage(const char* program) {
    std::cout << R"(
 ██╗  ██╗██╗   ██╗███╗   ███╗ █████╗ ███╗   ██╗
 ██║  ██║██║   ██║████╗ ████║██╔══██╗████╗  ██║
 ███████║██║   ██║██╔████╔██║███████║██╔██╗ ██║
 ██╔══██║██║   ██║██║╚██╔╝██║██╔══██║██║╚██╗██║
 ██║  ██║╚██████╔╝██║ ╚═╝ ██║██║  ██║██║ ╚████║
 ╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝
                                                 
  ██████╗██╗     ██╗ ██████╗██╗  ██╗███████╗██████╗ 
 ██╔════╝██║     ██║██╔════╝██║ ██╔╝██╔════╝██╔══██╗
 ██║     ██║     ██║██║     █████╔╝ █████╗  ██████╔╝
 ██║     ██║     ██║██║     ██╔═██╗ ██╔══╝  ██╔══██╗
 ╚██████╗███████╗██║╚██████╗██║  ██╗███████╗██║  ██║
  ╚═════╝╚══════╝╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
)" << "\n";
    std::cout << "Usage: " << program << " [options]\n\n"
        << "Distribution options:\n"
        << "  --normal [--default | <mean> <stddev>]        Normal distribution\n"
        << "  --gamma [--default | <shape> <scale>]         Gamma distribution (recommended)\n"
        << "  --lognormal [--default | <mean> <stddev>]     Log-normal distribution (note desired mean/stddev not mu/sigma)\n\n"
        << "Other options:\n"
        << "  -r, --right                                   Set to right click\n"
        << "  -t, --toggle <key>                            Set autoclicker toggle key, defaults to F6\n"
        << "  -db, --debug                                  Set debug messages of each click\n"
        << "  -d, --double [--default | <mean> <stddev>]    Set clicks to double click\n"
        << "  -h, --help                                    Show this help message\n\n"
        << "Valid toggle keys:\n"
        << "  Function: f1, f2, f3, ... f12\n"
        << "  Letters:  a-z\n"
        << "  Numbers:  0-9\n"
        << "  Special:  space, enter, shift, ctrl, alt, tab\n"
        << "  Numpad:   numpad0-numpad9\n"
        << "  Mouse:    mouseleft, mouseright, mousemiddle, mouse4, mouse5\n\n"
        << "Examples:\n"
        << "  " << program << " --gamma 3.0 27.7\n"
        << "  " << program << " --normal 83 15 --toggle mouseright\n"
        << "  " << program << " --uniform 60 100 --debug --r --double 10 2\n";
}

void DoClick(ClickType Click, bool Double, std::function<long()> double_ms)
{
    INPUT Inputs[2] = { 0 };
    if (Click == ClickType::RIGHT) {
        Inputs[0].type = INPUT_MOUSE;
        Inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        Inputs[1].type = INPUT_MOUSE;
        Inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    } else if (Click == ClickType::LEFT) {
        Inputs[0].type = INPUT_MOUSE;
        Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        Inputs[1].type = INPUT_MOUSE;
        Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    }

    SendInput(ARRAYSIZE(Inputs), Inputs, sizeof(INPUT));

    if (Double && (rand() > (RAND_MAX / 20))) // 95% success rate doubleclicking mouse
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(double_ms()));
        SendInput(ARRAYSIZE(Inputs), Inputs, sizeof(INPUT));
    }
}

void ToggleListener(int ToggleKey)
{
    while (true) {
        if (GetAsyncKeyState(ToggleKey) & 0x8000) {
            Clicking = !Clicking;
            std::cout << (Clicking ? "[ENABLED] Clicking active\n" : "[DISABLED] Clicking paused\n");

            // Wait for key release
            while (GetAsyncKeyState(ToggleKey) & 0x8000) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main(int argc, char* argv[]) {
    
    SetConsoleOutputCP(CP_UTF8);

    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printUsage(argv[0]);
        return 0;
    }

    //defaults 
    DistributionType DistType = DistributionType::NORMAL;
	float Mean = 80.0f; // 12cps
	float StdDev = 10.0f;

    float Shape = 4.0f; // 12cps
    float Scale = 20.0f; 

    ClickType Click = ClickType::LEFT;

    int ToggleKey = VK_F6;

    bool Double = false;
    float DoubleMean = 15.0f;
    float DoubleStdDev = 3.0f;

    bool Debug = false;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--normal") == 0) {
            DistType = DistributionType::NORMAL;
            if (strcmp(argv[i + 1], "--default") == 0)
                continue;
            Mean = std::stof(argv[i + 1]);
            StdDev = std::stof(argv[i + 2]);
            if (Mean <= 0 || StdDev <= 0)
            {
                std::cout << "Error: Invalid arguments for distribution type\n";
                return -1;
            }
        } else if (strcmp(argv[i], "--gamma") == 0) {
            DistType = DistributionType::GAMMA;
            if (strcmp(argv[i + 1], "--default") == 0)
                continue;
            Shape = std::stof(argv[i + 1]);
            Scale = std::stof(argv[i + 2]);
            if (Shape <= 0 || Scale <= 0)
            {
                std::cout << "Error: Invalid arguments for distribution type\n";
                return -1;
            }
        } else if (strcmp(argv[i], "--lognormal") == 0) {
            DistType = DistributionType::LOGNORMAL;
            if (strcmp(argv[i + 1], "--default") == 0)
                continue;
            Mean = std::stof(argv[i + 1]);
            StdDev = std::stof(argv[i + 2]);
            if (Mean <= 0 || StdDev <= 0)
            {
                std::cout << "Error: Invalid arguments for distribution type\n";
                return -1;
            }
        }
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--right") == 0)
        {
            Click = ClickType::RIGHT;
        }
        if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--toggle") == 0)
        {
            std::string ToggleKeyString = argv[i + 1]; // grab the togglekey
            std::transform(ToggleKeyString.begin(), ToggleKeyString.end(), ToggleKeyString.begin(), [](unsigned char c) { return std::tolower(c); }); // make it lower case
            ToggleKey = KeyMap[ToggleKeyString]; 
        }
        if (strcmp(argv[i], "-db") == 0 || strcmp(argv[i], "--debug") == 0)
        {
            Debug = true;
        }
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--double") == 0)
        {
            Double = true;
            if (strcmp(argv[i + 1], "--default") == 0)
                continue;
            DoubleMean = std::stof(argv[i + 1]);
            DoubleStdDev = std::stof(argv[i + 2]);
        }
    }
    
	std::random_device rd{};
	std::mt19937 gen{ rd() };

	std::normal_distribution<> NormalDist{ Mean, StdDev };
    std::lognormal_distribution<> LogNormalDist{ std::log(Mean), StdDev/Mean };
    std::gamma_distribution<> GammaDist{ Shape, Scale };
    
    // set the number generation
    std::function<long()> random_ms;

    switch (DistType)
    {
        case DistributionType::NORMAL:
            random_ms = [&NormalDist, &gen] { return std::lround(NormalDist(gen)); };
            break;
        case DistributionType::LOGNORMAL:
            random_ms = [&LogNormalDist, &gen] { return std::lround(LogNormalDist(gen)); };
            break;
        case DistributionType::GAMMA:
            random_ms = [&GammaDist, &gen] { return std::lround(GammaDist(gen)); };
            break;
        default:
            break;
    }

    // threading for toggle
    std::thread ToggleThread(ToggleListener, ToggleKey);
    ToggleThread.detach();

    // double click interval randomiser
	std::lognormal_distribution<> DoubleDist{ std::log(DoubleMean), DoubleStdDev/DoubleMean };
    std::function<long()> double_ms = [&DoubleDist, &gen]{ return std::lround(DoubleDist(gen)); };

    while (true)
    {
        if (Clicking)
        {
            long interval = random_ms();
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            
            if (Debug)
            {
                POINT p;
                GetCursorPos(&p);
                std::cout << "Interval: " << interval << "ms, at: " << p.x << ' ' << p.y << '\n';
            }

            DoClick(Click, Double, double_ms);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

	return 0;
}