#pragma once

#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <functional>
#include <thread>
#include <map>
#include <windows.h>

#pragma execution_character_set("utf-8")

enum class DistributionType {
    NORMAL,
    LOGNORMAL,
    GAMMA
};

enum class ClickType {
    LEFT,
    RIGHT
};

void printUsage(const char* program);
void DoClick(ClickType Click);
void ToggleListener(int ToggleKey);