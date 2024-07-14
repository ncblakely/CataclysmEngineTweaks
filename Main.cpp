#include "pch.h"
#include "Assembler.h"
#include "Config.h"
#include "GameHooks.h"
#include "PatchUniverse.h"
#include "PatchETG.h"
#include "PatchLOD.h"
#include "PatchRGL.h"
#include "PatchSaveGame.h"
#include "PatchSound.h"
#include "PatchAI.h"

#include "Cataclysm.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/dup_filter_sink.h"

static void ApplyStartupPatches(Config& config)
{
    if (config.BorderlessWindow)
    {
        *Globals::fullScreen = FALSE;
        *Globals::showBorder = FALSE;
    }

    if (config.DisableIntroMovies)
    {
        *Globals::enableAVI = FALSE;
    }
}

void ApplyPatches(Config& config)
{
    Assembler assembler;

    ApplyStartupPatches(config);
    InstallGameHooks(assembler, config);

    ApplyETGPatches(assembler, config);
    ApplyLODPatches(assembler, config);
    ApplyRGLPatches(assembler, config);
    ApplySaveGamePatches(assembler, config);
    ApplyUniversePatches(assembler, config);
    ApplySoundPatches(assembler, config);
    ApplyAIPatches(assembler, config);

    spdlog::info("Patches applied");
}

void InitLogger(std::string_view logFileName)
{
#ifdef _DEBUG
    // In debug mode, add an additional debug target
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFileName.data(), true);
    auto debug_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

    // Filter repeated messages
    auto dup_filter_sink = std::make_shared<spdlog::sinks::dup_filter_sink_mt>(std::chrono::milliseconds(100));
    dup_filter_sink->add_sink(file_sink);
    dup_filter_sink->add_sink(debug_sink);

    auto multi_logger = std::shared_ptr<spdlog::logger>(new spdlog::logger("default", dup_filter_sink));

    spdlog::set_default_logger(multi_logger);
#else
    // Filter repeated messages
    auto dup_filter_sink = std::make_shared<spdlog::sinks::dup_filter_sink_mt>(std::chrono::milliseconds(100));
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFileName.data(), true);

    dup_filter_sink->add_sink(file_sink);

    auto logger = std::shared_ptr<spdlog::logger>(new spdlog::logger("default", dup_filter_sink));
    spdlog::set_default_logger(logger);
#endif
    spdlog::default_logger()->flush_on(spdlog::level::trace);
    spdlog::default_logger()->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
}