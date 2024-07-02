#include "pch.h"
#include "Main.h"
#include "Assembler.h"
#include "Config.h"
#include "Cataclysm.h"

#include "rinit.h"

void rinSortModes(rdevice* dev);
HRESULT WINAPI rinEnumDisplayModes_cb(LPDDSURFACEDESC2 ddsd, LPVOID lpContext);
HRESULT WINAPI rinEnumPrimaryDisplayModes_cb(LPDDSURFACEDESC ddsd, LPVOID lpContext);

constexpr const char* OpenGlDllName = "opengl32.dll";

static int s_primaryWidth = 640;
static int s_primaryHeight = 480;

void GetPrimaryMonitorResolution()
{
    RECT desktopRect;
    HWND hDesktop = GetDesktopWindow();
    GetClientRect(hDesktop, &desktopRect);

    // Save these to statics so we can use in InitWindow_DeviceCrcCheck
    s_primaryWidth = desktopRect.right;
    s_primaryHeight = desktopRect.bottom;
}

static void* InitWindow_DeviceCrcCheck_Continue = (void*)0x00563645;
static void __declspec(naked) InitWindow_DeviceCrcCheck()
{
    using namespace Functions;
    using namespace Globals;

    __asm
    {
        pushad
    }

    if (*opDeviceCRC != rinDeviceCRC())
    {
        // Normally, HWC will default to software mode if the device CRC changed or isn't set.
        // Update the saved device CRC and select OpenGL mode with the primary monitor's resolution instead

        *opDeviceCRC = rinDeviceCRC();

        *MAIN_WindowWidth = s_primaryWidth;
        *MAIN_WindowHeight = s_primaryHeight;
        *MAIN_WindowDepth = 32;

        mainRescaleMainWindow();

        strcpy_s(glToSelect, 512, OpenGlDllName);
    }

    __asm
    {
        popad
        jmp InitWindow_DeviceCrcCheck_Continue
    }
}

static Functions::fn_glCapStartup orig_glCapStartup;
static void glCapStartup()
{
    using namespace Globals;

    orig_glCapStartup();

    if (g_Config.NoCompiledVertexArrays)
    {
        *glCapCompiledVertexArray = FALSE;
    }
}

static void CreateAndEnableHooks(Assembler& assembler, Config& config)
{
    void* _discard = nullptr;

    if (config.NewRendererSelection)
    {
        CreateAndEnableHook(Functions::rinSortModes, rinSortModes, &_discard);
        CreateAndEnableHook(Functions::rinEnumDisplayModes_cb, rinEnumDisplayModes_cb, &_discard);
        CreateAndEnableHook(Functions::rinEnumPrimaryDisplayModes_cb, rinEnumPrimaryDisplayModes_cb, &_discard);
        CreateAndEnableHook(Instructions::InitWindow_DeviceCrcCheck, InitWindow_DeviceCrcCheck, &_discard);

        GetPrimaryMonitorResolution();
    }

    // Disable renderer re-initialization on WM_ACTIVATE in windowed mode; this is unnecessary and causes alt-tabs to take longer than they should
    if (!*Globals::fullScreen)
    {
        assembler.Write("jmp 0x0056208E", Instructions::ActivateMe_ReInitRenderer);
    }

    if (config.DisableFramerateLimit)
    {
        assembler.Write("nop; nop; nop; nop; nop; nop; nop; nop", (void*)0x0057B78B);
        assembler.Write("nop; nop; nop; nop; nop; nop; nop; nop", (void*)0x0057B7A1);
    }

    CreateAndEnableHook(Functions::glCapNT, glCapNT, &_discard);
    CreateAndEnableHook(Functions::glCapStartup, glCapStartup, &orig_glCapStartup);
}

void ApplyRGLPatches(Assembler& assembler, Config& config)
{
    CreateAndEnableHooks(assembler, config);

    spdlog::info("RGL patches applied");
}