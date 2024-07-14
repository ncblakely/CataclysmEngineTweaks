#pragma once

#include "Config.h"

constexpr const char* ConfigFileName = "CataclysmEngineTweaks.ini";

void ApplyPatches(Config& config);
void InitLogger(std::string_view logFileName);

////////////////////////////////////////////////////////////////////////
// Utilities

template<typename... Args>
void LogAndThrow(fmt::basic_string_view<char> fmt, Args&&... args)
{
    auto message = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
    spdlog::error(message);
    spdlog::default_logger()->flush();

    throw std::runtime_error(message);
}

template<typename T>
void CreateAndEnableHook(void* target, void* detour, T** original)
{
    MH_STATUS status = MH_CreateHook(target, detour, reinterpret_cast<void**>(original));
    if (status != MH_OK)
    {
        LogAndThrow("Failed to create hook: {}", MH_StatusToString(status));
    }

    if (MH_EnableHook(target) != MH_OK)
    {
        LogAndThrow("Failed to enable hook");
    }
}

inline std::string FloatToHex(float value)
{
    return fmt::format("{:#x}", *reinterpret_cast<sdword*>(&value));
}

inline std::string IntToHex(sdword value)
{
    return fmt::format("{:#x}", *reinterpret_cast<sdword*>(&value));
}

template<typename T>
inline T Clamp(T min, T max, T value)
{
    return std::max(min, std::min(max, value));
}

////////////////////////////////////////////////////////////////////////