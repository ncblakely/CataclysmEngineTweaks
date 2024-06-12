#include "pch.h"
#include "Main.h"
#include "Assembler.h"
#include "Config.h"
#include "Cataclysm.h"

#include "rinit.h"

void rinSortModes(rdevice* dev);
HRESULT WINAPI rinEnumDisplayModes_cb(LPDDSURFACEDESC2 ddsd, LPVOID lpContext);
HRESULT WINAPI rinEnumPrimaryDisplayModes_cb(LPDDSURFACEDESC ddsd, LPVOID lpContext);

static void CreateAndEnableHooks(Assembler& assembler, Config& config)
{
    void* _discard = nullptr;

    CreateAndEnableHook(Functions::rinSortModes, rinSortModes, &_discard);
    CreateAndEnableHook(Functions::rinEnumDisplayModes_cb, rinEnumDisplayModes_cb, &_discard);
    CreateAndEnableHook(Functions::rinEnumPrimaryDisplayModes_cb, rinEnumPrimaryDisplayModes_cb, &_discard);
}

void ApplyRGLPatches(Assembler& assembler, Config& config)
{
    if (!config.IsNewResolutionPickerEnabled())
    {
        return;
    }

    CreateAndEnableHooks(assembler, config);

    spdlog::info("RGL patches applied");
}