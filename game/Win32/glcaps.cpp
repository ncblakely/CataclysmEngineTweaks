/*=============================================================================
        Name    : glcaps.c
        Purpose : determine the capabilities of the GL currently in use

        Created 19/06/1998 by khent
        Copyright Relic Entertainment, Inc.  All rights reserved.
=============================================================================*/

#include "pch.h"

#include "glcaps.h"
#include "Cataclysm.h"

/*-----------------------------------------------------------------------------
    Name        : glCapNT
    Description : determines whether running NT <= 4
    Inputs      :
    Outputs     :
    Return      : TRUE or FALSE (NT <= 4 or not)
----------------------------------------------------------------------------*/
BOOL glCapNT(void)
{
    // If the OpenGL fix is enabled, just return true here rather than doing the old version check below.
    // This causes Homeworld to use the newer Windows NT-based APIs in opengl32.dll, fixing compatibility on newer Windows versions.
    if (g_Config.OpenGLFix)
    {
        return TRUE;
    }

// Disable deprecation warning
#pragma warning (disable:4996)
    OSVERSIONINFO osVer;
    osVer.dwOSVersionInfoSize = sizeof(osVer);
    if (!GetVersionEx(&osVer))
    {
        return TRUE;
    }
    if (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT &&
        osVer.dwMajorVersion <= 4)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#pragma warning (default:4996)
}