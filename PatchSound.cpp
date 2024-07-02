#include "pch.h"

#include "Cataclysm.h"
#include "PatchSound.h"
#include "soundlow.h"

#define SOUND_ERR		-1
#define SOUND_OK		0

void CreateAndEnableHooks()
{
	void* _discard = nullptr;

	if (!g_Config.ForceWaveOut)
	{
		CreateAndEnableHook(Functions::soundStartDSound, soundStartDSound, &_discard);
	}
}

void ApplySoundPatches(Assembler& assembler, Config& config)
{
	CreateAndEnableHooks();

	if (g_Config.DisableEqualizer)
	{
		assembler.Write("ret", Functions::fqEqualize);
	}
}