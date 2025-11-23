#include "main.h"

std::atomic<bool> Clicking = true;
std::vector<uint16_t> ClickInterval;
std::vector<uint8_t> DoubleInterval;
std::mutex HistogramMutex;

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
        << "  -m, --min <ms>                                Set minimum click interval (default to 60ms)\n"
        << "  -g, --graph [-v | --vertical]                 Show a pretty histogram of the click statistics\n"
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
        << "  " << program << " --lognormal 60 100 -db -r -d 10 2\n";
}

void DisplayHistogram(bool Vertical) {
    std::lock_guard<std::mutex> lock(HistogramMutex);

    if (ClickInterval.empty()) return;

    system("cls");

    std::map<uint16_t, int> Bins;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int ConsoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int ConsoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    uint16_t Min = *std::min_element(ClickInterval.begin(), ClickInterval.end());
    uint16_t Max = *std::max_element(ClickInterval.begin(), ClickInterval.end());
    uint16_t Range = Max - Min;
    int MaxBarWidth = ConsoleWidth - 20;
    int MaxBarHeight = ConsoleHeight - 5;

    uint16_t BinSize;
    if (Range < 25 || (Vertical && Range < 60)) {
        BinSize = 1;          
    }
    else if (Range < 50 || (Vertical && Range < 120)) {
        BinSize = 2;         
    }
    else if (Range < 125 || (Vertical && Range < 300)) {
        BinSize = 5;         
    }
    else if (Range < 500) {
        BinSize = 20;        
    }
    else {
        BinSize = 100;        
    }

    for (auto val : ClickInterval)
    {
        Bins[val / BinSize * BinSize]++;
    }

    uint16_t MinBin = (Min / BinSize) * BinSize;
    uint16_t MaxBin = (Max / BinSize) * BinSize;
    for (uint16_t bin = MinBin; bin <= MaxBin; bin += BinSize) {
        if (Bins.find(bin) == Bins.end()) {
            Bins[bin] = 0;  // Add empty bin
        }
    }

    int MaxBinValue = Bins.rbegin()->first;
    int ColWidth = std::to_string(MaxBinValue).length() + 1;  // +1 for spacing

    int MaxCount = 0;
    for (const auto& [bin, count] : Bins)
    {
        MaxCount = max(MaxCount, count);
    }

    int ScaleWidth = std::to_string(MaxCount).length() + 3;

    std::stringstream ss;

    if (Vertical) {
        for (int row = MaxBarHeight; row >= 0; row--) {
            // Print scale label
            bool printedNumber = false;
            std::string scaleLabel = std::string(ScaleWidth, ' ');

            // Check each bin to see if this row is the top of a bar
            for (const auto& [bin, count] : Bins) {
                int barHeight = (count * MaxBarHeight) / MaxCount;
                if (row == barHeight - 1 && !printedNumber) {
                    scaleLabel = std::to_string(count);
                    // Right-align the number
                    scaleLabel = std::string(ScaleWidth - scaleLabel.length(), ' ') + scaleLabel;
                    printedNumber = true;
                    break;
                }
            }

            ss << scaleLabel << " |";

            // Print bars
            for (const auto& [bin, count] : Bins) {
                int barHeight = (count * MaxBarHeight) / MaxCount;
                if (row < barHeight) {
                    int padding = (ColWidth - 1) / 2;
                    ss << std::string(padding, ' ') << '#' << std::string(ColWidth - padding - 1, ' ');
                }
                else {
                    ss << std::string(ColWidth, ' ');
                }
            }
            ss << "\n";
        }

        // Bottom separator with corner
        ss << std::string(ScaleWidth, ' ') << "+" << std::string(Bins.size() * ColWidth, '-') << "\n";

        // Bin labels
        ss << std::string(ScaleWidth + 1, ' ');
        for (const auto& [bin, count] : Bins) {
            ss << std::setw(ColWidth) << bin;
        }
        ss << " (ms)\n";
    } else {
        for (const auto& [bin, count] : Bins) 
        {
            int barLength = (count * MaxBarWidth) / MaxCount; // scale to 50 chars
            ss << std::setw(4) << bin << "ms | ";
            ss << std::string(barLength, '#') << " " << count << "\n";
        }
    }
    std::cout << ss.str();
}

void DoClick(ClickType Click, bool Double, std::function<uint8_t()> double_ms, bool Graph)
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
        uint8_t DoubleClickInterval = double_ms();

        if (Graph)
        {
            {
                std::lock_guard<std::mutex> lock(HistogramMutex);
                DoubleInterval.push_back(DoubleClickInterval);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(DoubleClickInterval));
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
    uint16_t MinClickTime = 60;
    
    DistributionType DistType = DistributionType::NORMAL;
	float Mean = 80.0f; // 12cps
	float StdDev = 10.0f;

    float Shape = 16.0f; // 12cps
    float Scale = 6.0f; 

    ClickType Click = ClickType::LEFT;

    int ToggleKey = VK_F6;

    bool Double = false;
    float DoubleMean = 15.0f;
    float DoubleStdDev = 3.0f;

    bool Debug = false;

    bool Graph = false;
    bool VerticalGraph = false;

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
        if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--graph") == 0)
        {
            Graph = true;
            if (strcmp(argv[i + 1], "-v") == 0 || strcmp(argv[i + 1], "--vertical") == 0)
            {
                VerticalGraph = true;
            }
        }
        if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--min") == 0)
        {
            MinClickTime = static_cast<uint16_t>(std::stoi(argv[i + 1]));
        }
    }
    
	std::random_device rd{};
	std::mt19937 gen{ rd() };

	std::normal_distribution<> NormalDist{ Mean, StdDev };
    std::lognormal_distribution<> LogNormalDist{ std::log(Mean), StdDev/Mean };
    std::gamma_distribution<> GammaDist{ Shape, Scale };
    
    // set the number generation
    std::function<uint16_t()> random_ms;

    switch (DistType)
    {
        case DistributionType::NORMAL:
            random_ms = [&NormalDist, &gen] { return static_cast<uint16_t>(std::lround(NormalDist(gen))); };
            break;
        case DistributionType::LOGNORMAL:
            random_ms = [&LogNormalDist, &gen] { return static_cast<uint16_t>(std::lround(LogNormalDist(gen))); };
            break;
        case DistributionType::GAMMA:
            random_ms = [&GammaDist, &gen] { return static_cast<uint16_t>(std::lround(GammaDist(gen))); };
            break;
        default:
            break;
    }

    // threading for toggle
    std::thread ToggleThread(ToggleListener, ToggleKey);
    ToggleThread.detach();

    // double click interval randomiser
	std::lognormal_distribution<> DoubleDist{ std::log(DoubleMean), DoubleStdDev/DoubleMean };
    std::function<uint8_t()> double_ms = [&DoubleDist, &gen]{ return static_cast<uint8_t>(std::lround(DoubleDist(gen))); };

    while (true)
    {
        if (Clicking)
        {
            uint16_t Interval;
            do { Interval = random_ms(); } while (Interval < MinClickTime);

            if (Graph)
            {
                ClickInterval.push_back(Interval);
                DisplayHistogram(VerticalGraph);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(Interval));

            if (Debug)
            {
                POINT p;
                GetCursorPos(&p);
                std::cout << "Interval: " << Interval << "ms, at: " << p.x << ' ' << p.y << '\n';
            }

            DoClick(Click, Double, double_ms, Graph);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

	return 0;
}