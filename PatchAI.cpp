#include "pch.h"
#include "Main.h"

#include "Cataclysm.h"
#include "AIPlayer.h"

#include "PatchAI.h"

static Functions::fn_aisFleetUpdate orig_aisFleetUpdate = nullptr;
static void aisFleetUpdateDebug()
{
	DumpAITeams();
	orig_aisFleetUpdate();
}

static void CreateAndEnableHooks(Assembler& assembler, Config& config)
{
	void* _discard = nullptr;

	if (g_Config.EnableNewAI)
	{
		CreateAndEnableHook(Functions::aisFleetUpdate, aisFleetUpdate, &_discard);
	}
#ifdef _DEBUG
	else
	{
		CreateAndEnableHook(Functions::aisFleetUpdate, aisFleetUpdateDebug, &orig_aisFleetUpdate);
	}
#endif
}

void ApplyAIPatches(Assembler& assembler, Config& config)
{
	CreateAndEnableHooks(assembler, config);
}