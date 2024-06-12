#include "pch.h"

#include "Utilities.h"

void KillProcessesByName(const char* exeName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        DWORD lastError = GetLastError();
        printf("CreateToolhelp32Snapshot failed (%d).\n", lastError);
        return;
    }

    PROCESSENTRY32 pe32{};
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32))
    {
        DWORD lastError = GetLastError();
        printf("Process32First failed (%d).\n", lastError);
        CloseHandle(hSnapshot);
        return;
    }

    do
    {
        if (strcmp(pe32.szExeFile, exeName) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (hProcess)
            {
                printf("Ending process with ID %d.\n", pe32.th32ProcessID);

                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return;
}