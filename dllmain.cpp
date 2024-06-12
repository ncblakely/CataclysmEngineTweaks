// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Config.h"

Config g_Config;

static void CheckModuleLoadAddress()
{
    static DWORD ExpectedImageBase = 0x400000;

    HMODULE loadAddress = GetModuleHandle(nullptr);
    if ((DWORD)loadAddress != ExpectedImageBase)
    {
        LogAndThrow("CET failed to load due to unexpected image base {:x}; relocation possibly enabled?");
    }
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            try
            {
                CheckModuleLoadAddress();

                if (MH_Initialize() != MH_OK)
                {
                    LogAndThrow("Failed to initialize MinHook");
                }

                InitLogger();
                g_Config = Config::FromFile(ConfigFileName);

                ApplyPatches(g_Config);
            }
            catch (std::exception& e)
			{
				spdlog::error("An error occurred during initialization: {}", e.what());

#ifdef _DEBUG
                __debugbreak();
#endif
                return FALSE;
			}

            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            spdlog::default_logger()->flush();

            MH_Uninitialize();
            break;
        }
        return TRUE;
}

