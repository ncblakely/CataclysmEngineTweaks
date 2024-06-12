/*=============================================================================
    Name    : rinit.cpp
    Purpose : rGL / OpenGL enumeration initialization routines

    Created 1/5/1999 by khent
    Copyright Relic Entertainment, Inc.  All rights reserved.
=============================================================================*/

#include "pch.h"
#include "Cataclysm.h"
#include "rinit.h"

/*-----------------------------------------------------------------------------
    Name        : rinAddMode
    Description : add a display mode to a video driver
    Inputs      : dev - the device to add the mode to
                  width, height - dimensions
                  depth - bitdepth
    Outputs     :
    Return      :
----------------------------------------------------------------------------*/
void rinAddMode(rdevice* dev, int width, int height, int depth)
{
    rmode* mode;
    rmode* cmode;

    mode = (rmode*)Functions::memAlloc(sizeof(rmode));
    mode->width = width;
    mode->height = height;
    mode->depth = depth;

    if (dev->modes == NULL)
    {
        //no modes yet
        dev->modes = mode;
        mode->next = NULL;
    }
    else
    {
        //add to tail
        cmode = dev->modes;
        while (cmode->next != NULL)
        {
            cmode = cmode->next;
        }
        cmode->next = mode;
        mode->next = NULL;
    }
}

/*-----------------------------------------------------------------------------
    Name        : rinModeAccepted
    Description : decides whether a given mode is supported
    Inputs      : dev - the device in question
                  width, height, depth - display mode characteristics
    Outputs     :
    Return      : true or false
----------------------------------------------------------------------------*/
static bool rinModeAccepted(rdevice* dev, int width, int height, int depth)
{
    // accept 32 bit mode only
    if (depth != 32)
    {
        return false;
    }

    return true;
}

void rinSortModes(rdevice* dev)
{
    rmode* cmode;
    rmode* freeMode;
    rdevice dummy;

    if (dev->modes == NULL)
    {
        return;
    }

    memset(&dummy, 0, sizeof(rdevice));

    cmode = dev->modes;
    do
    {
        if (rinModeAccepted(dev, cmode->width, cmode->height, cmode->depth))
        {
            rinAddMode(&dummy, cmode->width, cmode->height, cmode->depth);
        }

        cmode = cmode->next;
    } while (cmode != NULL);

    //free modes on dev
    cmode = dev->modes;
    while (cmode != NULL)
    {
        freeMode = cmode;    //save to free
        cmode = cmode->next; //next
        Functions::memFree(freeMode);   //free
    }

    //attach new modes
    dev->modes = dummy.modes;
}

/*-----------------------------------------------------------------------------
    Name        : rinEnumDisplayModes_cb
    Description : callback during display mode enumeration
    Inputs      : ...
    Outputs     :
    Return      :
----------------------------------------------------------------------------*/
HRESULT WINAPI rinEnumDisplayModes_cb(LPDDSURFACEDESC2 ddsd, LPVOID lpContext)
{
    rdevice* dev;

    dev = (rdevice*)lpContext;

    if ((ddsd->dwFlags & DDSD_WIDTH) &&
        (ddsd->dwFlags & DDSD_HEIGHT) &&
        (ddsd->dwFlags & DDSD_PIXELFORMAT))
    {
        if (ddsd->ddpfPixelFormat.dwRGBBitCount != 32)
        {
            goto SKIP_MODE;
        }
        if (ddsd->dwWidth < 640 || ddsd->dwHeight < 480)
        {
            goto SKIP_MODE;
        }

        rinAddMode(dev,
            ddsd->dwWidth, ddsd->dwHeight,
            ddsd->ddpfPixelFormat.dwRGBBitCount);
    }

SKIP_MODE:
    return DDENUMRET_OK;
}

/*-----------------------------------------------------------------------------
    Name        : rinEnumPrimaryDisplayModes_cb
    Description : callback during display mode enumeration, old DDraw
    Inputs      : ...
    Outputs     :
    Return      :
----------------------------------------------------------------------------*/
HRESULT WINAPI rinEnumPrimaryDisplayModes_cb(LPDDSURFACEDESC ddsd, LPVOID lpContext)
{
    rdevice* dev;

    dev = (rdevice*)lpContext;

    if (dev == NULL)
    {
        goto SKIP_MODE;
    }

    if ((ddsd->dwFlags & DDSD_WIDTH) &&
        (ddsd->dwFlags & DDSD_HEIGHT) &&
        (ddsd->dwFlags & DDSD_PIXELFORMAT))
    {
        if (ddsd->ddpfPixelFormat.dwRGBBitCount != 16 &&
            ddsd->ddpfPixelFormat.dwRGBBitCount != 32)
        {
            goto SKIP_MODE;
        }
        if (ddsd->dwWidth < 640 || ddsd->dwHeight < 480)
        {
            goto SKIP_MODE;
        }

        rinAddMode(dev,
            ddsd->dwWidth, ddsd->dwHeight,
            ddsd->ddpfPixelFormat.dwRGBBitCount);
    }

SKIP_MODE:
    return DDENUMRET_OK;
}