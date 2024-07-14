#include "pch.h"
#include "Main.h"

#include "Cataclysm.h"
#include "AIPlayer.h"

#include "PatchAI.h"

static udword SAI_MIMIC_TARGET_COUNT[AI_NUM_LEVELS] = {0, 0, 14};

scriptEntry AISkirmishTweaks[] =
{
	makeEntry(SAI_MIMIC_TARGET_COUNT[AI_BEG], Functions::scriptSetUdwordCB),
	makeEntry(SAI_MIMIC_TARGET_COUNT[AI_INT], Functions::scriptSetUdwordCB),
	makeEntry(SAI_MIMIC_TARGET_COUNT[AI_ADV], Functions::scriptSetUdwordCB),

	endEntry
};

static void CreateAndEnableHooks(Assembler& assembler, Config& config)
{
	void* _discard = nullptr;

	CreateAndEnableHook(Functions::aisFleetUpdate, aisFleetUpdate, &_discard);
}

void ApplyAIPatches(Assembler& assembler, Config& config)
{
	if (g_Config.EnableNewAI)
	{
		CreateAndEnableHooks(assembler, config);
	}
}