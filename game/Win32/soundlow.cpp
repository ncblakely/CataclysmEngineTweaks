/*=============================================================================
    Name    : SoundLow.c
    Purpose : Low level sound routines

    Created 7/24/1997 by gshaw
    Copyright Relic Entertainment, Inc.  All rights reserved.
=============================================================================*/

#include "pch.h"

#include "Cataclysm.h"

#define SOUND_ERR		-1
#define SOUND_OK		0

sdword soundStartDSound(HWND hWnd)
{
	using namespace Globals;

	DSBUFFERDESC dsbdesc;
	DSCAPS dscaps;
	HRESULT hr;

	if (DS_OK == DirectSoundCreate(NULL, lpDirectSound, NULL))
	{
		// Set up DSBUFFERDESC structure.
		memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

		// Buffer size is determined by sound hardware.
		dsbdesc.dwBufferBytes = 0;
		dsbdesc.lpwfxFormat = NULL; // Must be NULL for primary buffers.

		if (*coopDSound)
		{
			// Try to set in priority mode so Homeworld will share the wave device
			hr = (*lpDirectSound)->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
			if (hr != DS_OK)
			{
				// Hmmm, couldn't set priority so lets try exclusive
				hr = (*lpDirectSound)->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE);
			}
		}
		else
		{
			// Try to set in Exclusive mode
			hr = (*lpDirectSound)->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE);
			if (hr != DS_OK)
			{
				// maybe something already has the wave device so lets try sharing it?
				hr = (*lpDirectSound)->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
			}
		}

		if (hr == DS_OK)
		{
			// Get direct sound Caps.
			dscaps.dwSize = sizeof(DSCAPS);
			if (DS_OK == (*lpDirectSound)->GetCaps(&dscaps))
			{
#ifndef HW_Release
				dbgMessagef("\n*****  DIRECT SOUND CAPS  *****");
#endif
				if (true /* || (dscaps.dwFlags & DSCAPS_CERTIFIED) || useDSound */)
				{
#ifndef HW_Release
					dbgMessagef("\nDriver is CERTIFIED");
#endif
					* bDirectSoundCertified = TRUE;
				}
				else 	// if (dscaps.dwFlags & DSCAPS_EMULDRIVER)
				{
#ifndef HW_Release
					dbgMessagef("\nDriver not certified, using WAVEOUT");
#endif
					(*lpDirectSound)->Release();
					return (SOUND_ERR);
				}

				// Succeeded. Try to create buffer.
				if (DS_OK == (*lpDirectSound)->CreateSoundBuffer(&dsbdesc, lpPrimaryBuffer, NULL))
				{
					// Succeeded. Set primary buffer to desired format.
					if (DS_OK == (*lpPrimaryBuffer)->SetFormat(waveFormatEX))
					{
						spdlog::info("DSound init OK");

						return (SOUND_OK);
					}
					else
					{
						(*lpPrimaryBuffer)->Release();
					}
				}
			}
			else
			{
				(*lpDirectSound)->Release();
			}
		}
		// SetCooperativeLevel failed.
		// CreateSoundBuffer, or SetFormat.
		lpPrimaryBuffer = NULL;
	}

	spdlog::error("DSound init failed: 0x{:x}", hr);
	return (SOUND_ERR);
}