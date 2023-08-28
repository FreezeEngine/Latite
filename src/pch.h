#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string_view>
#include <memory>
#include <future> 
#include <execution>
#include <optional>
#include <chrono>

using namespace std::chrono_literals;

// COM
#include <d2d1.h>
#include <d3d11.h>
#include <d3d11on12.h>
#include <d3d12.h>
//#include <winrt/base.h>
