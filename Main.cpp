#include "pch.h"
#include "Assembler.h"
#include "Config.h"
#include "GameHooks.h"
#include "PatchUpdateRate.h"
#include "PatchETG.h"
#include "PatchLOD.h"
#include "PatchRGL.h"

#include "Cataclysm.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

static void ApplyStartupPatches(Config& config)
{
    if (config.IsBorderlessWindowEnabled())
    {
        *Globals::fullScreen = FALSE;
        *Globals::showBorder = FALSE;
    }

    if (config.IsIntroMovieDisabled())
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
    
    ApplyUpdateRatePatch(assembler, config);

    spdlog::info("Patches applied");
}

void InitLogger()
{
#ifdef _DEBUG
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("CataclysmEngineTweaks.log", true);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto multi_sink = std::shared_ptr<spdlog::logger>(new spdlog::logger("default", {console_sink, file_sink}));

    spdlog::set_default_logger(multi_sink);
#else
    auto basic_logger_mt = spdlog::basic_logger_mt("default", "CataclysmEngineTweaks.log", true);
    spdlog::set_default_logger(basic_logger_mt);
#endif
    spdlog::default_logger()->flush_on(spdlog::level::trace);

    spdlog::info("Cataclysm Engine Tweaks loaded");
}