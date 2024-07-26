// Simple loader application to inject CataclysmEngineTweaks.dll and start the process.
#include "pch.h"

#include "Utilities.h"

static const char* ExeName = "cataclysm.exe";
static const char* DllName = "CataclysmEngineTweaks.dll";

static bool CheckCurrentDirectory()
{
    using namespace std::filesystem;

    if (!exists("CataclysmEngineTweaks.dll"))
    {
        printf("CataclysmEngineTweaks.dll not found in the current directory.\n");
        return false;
    }

    if (!exists("cataclysm.exe"))
    {
        printf("cataclysm.exe not found in the current directory.\n");
        return false;
    }

    return true;
}

static std::string BuildCommandLine(int argc, char* argv[])
{
    std::string commandLine;
    for (int i = 1; i < argc; ++i)
    {
        commandLine += argv[i];
        commandLine += " ";
    }

    return std::format("cataclysm.exe {}", commandLine);
}

int InjectDll(const PROCESS_INFORMATION& processInformation)
{
    LPVOID pDllPath = VirtualAllocEx(processInformation.hProcess, 0, strlen(DllName) + 1, MEM_COMMIT, PAGE_READWRITE);

    if (!pDllPath)
    {
        DWORD lastError = GetLastError();
        printf("VirtualAllocEx failed (%d).\n", lastError);
        return lastError;
    }

    if (!WriteProcessMemory(processInformation.hProcess, pDllPath, (LPVOID)DllName, strlen(DllName) + 1, 0))
    {
        DWORD lastError = GetLastError();
        printf("WriteProcessMemory failed (%d).\n", lastError);
        return lastError;
    }

    HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
    if (!hKernel32)
    {
        DWORD lastError = GetLastError();
        printf("GetModuleHandle failed (%d).\n", lastError);
        return lastError;
    }

    HANDLE hLoadThread = CreateRemoteThread(processInformation.hProcess, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA"), pDllPath, 0, 0);
    if (!hLoadThread)
    {
        DWORD lastError = GetLastError();
		printf("CreateRemoteThread failed (%d).\n", lastError);
		return lastError;
    }

    WaitForSingleObject(hLoadThread, INFINITE);

    return 0;
}

int main(int argc, char* argv[])
{
    PROCESS_INFORMATION processInformation{};
    STARTUPINFO startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    if (!CheckCurrentDirectory())
	{
		return 1;
	}

    // Stop any existing game processes; otherwise, if we fail to launch, we might start multiple processes in suspended mode.
    KillProcessesByName(ExeName);

    std::string commandLine = BuildCommandLine(argc, argv);

    // Start game process in suspended mode so we can inject the DLL before invoking main()
    if (!CreateProcess(nullptr,
        (char*)commandLine.c_str(),
        nullptr, 
        nullptr,
        FALSE,
        CREATE_SUSPENDED,
        nullptr,
        nullptr, // Use current working directory
        &startupInfo,
        &processInformation))
    {
        DWORD lastError = GetLastError();
        printf("CreateProcess failed (%d).\n", lastError);
        return lastError;
    }

    int exitCode = InjectDll(processInformation);
    if (exitCode != 0)
    {
        return exitCode;
    }

    ResumeThread(processInformation.hThread);

    return 0;
}
