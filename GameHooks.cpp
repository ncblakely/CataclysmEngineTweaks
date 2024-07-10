// Defines shared hook for game functions that are always active, regardless of config settings.

#include "pch.h"
#include "GameHooks.h"
#include "GameState.h"

#include "Cataclysm.h"

constexpr const char* NetworkVersion = "v1.02";
constexpr const char* MinorBuildVersion = "0";

static Functions::fn_univUpdate orig_univUpdate;
static bool univUpdate(real32 phystimeelapsed)
{
	using namespace Globals;
	using namespace Functions;

	if (g_GameState.autosaveTimer.has_value())
	{
		*g_GameState.autosaveTimer -= phystimeelapsed;

		if (g_GameState.autosaveTimer <= 0.0f)
		{
			const char* folder = *singlePlayerGame ? "SinglePlayer" : "MultiPlayer";
			if (Functions::SaveGame(fmt::format("SavedGames\\{}\\Autosave", folder).c_str()))
			{
				clCommandMessage("Game Autosaved", -1);
			}
			else
			{
				clCommandMessage("Autosave failed!", -1);
			}

			g_GameState.ResetAutosaveTimer();
		}
	}

	return orig_univUpdate(phystimeelapsed);
}

static Functions::fn_opOptionsAccept orig_opOptionsAccept;
static void opOptionsAccept(char* name, featom* atom)
{
	using namespace Globals;

	orig_opOptionsAccept(name, atom);

	// Ensure the aspect ratio is updated when the resolution changes
	*rndAspectRatio = (real32)*MAIN_WindowWidth / (real32)*MAIN_WindowHeight;
}

static void __declspec(naked) LoadMission_AdjustTextureMemoryLimit()
{
	using namespace Globals;

	if (g_Config.UnlimitedVideoMemory)
	{
		__asm
		{
			mov eax, INT32_MAX
			mov dword ptr ss: [esp+0x18], eax // Set the texture memory limit to INT32_MAX
		}
	}

	// Jump back, skipping over the code that limits the VRAM pool on SP mission 17
	__asm jmp dword ptr ds: [Instructions::LoadMission_AfterTextureMemoryLimitAdjustment]
}

static void* orig_CheckPlayerWin_GameTypeCheck;
static void __declspec(naked) CheckPlayerWin_GameTypeCheck()
{
	using namespace Globals;
	using namespace Instructions;

	if (!*multiPlayerGame && !*singlePlayerGame)
	{
		// Fix win condition for skirmish games: in skirmish, neither variable is true. 
		// Jump to the win condition case for multiplayer games, which works properly.
		__asm
		{
			jmp dword ptr ds: [CheckPlayerWin_IsMultiplayerGame]
		}
	}
	else
	{
		__asm jmp orig_CheckPlayerWin_GameTypeCheck
	}
}

void InstallGameHooks(Assembler& assembler, Config& config)
{
	void* _discard = nullptr;

	CreateAndEnableHook(Functions::univUpdate, univUpdate, &orig_univUpdate);
	CreateAndEnableHook(Functions::opOptionsAccept, opOptionsAccept, &orig_opOptionsAccept);
	CreateAndEnableHook(Instructions::CheckPlayerWin_GameTypeCheck, CheckPlayerWin_GameTypeCheck, &orig_CheckPlayerWin_GameTypeCheck);
	CreateAndEnableHook(Instructions::LoadMission_AdjustTextureMemoryLimit, LoadMission_AdjustTextureMemoryLimit, &_discard);

	//////////////////////////////////////////////
	// Version number
	using namespace Globals;

	strcpy(networkVersion, NetworkVersion);
	strcpy(minorBuildVersion, MinorBuildVersion);

	//////////////////////////////////////////////
	// Bug fixes:

	// Skip some buggy code in partRenderBillSystem that can cause non-particle textures to switch to point filtering at random.
	assembler.Write(
		fmt::format("jmp 0x{:x}", (udword)Instructions::partRenderBillSystem_AfterSetPointFiltering),
		Instructions::partRenderBillSystem_SetPointFiltering);

	// Fix F12 key up events not being recognized: disable debug key handler for F12
	assembler.Write(
		fmt::format("je 0x{:x}", udword(Instructions::WindowProc_NormalKeyUpEvent)),
		Instructions::WindowProc_JumpIfF12KeyUp);
	
	// int3 breakpoints are enabled by default, which prevents the assertion failure message from showing.
	// Turn this off.
	*dbgInt3Enabled = FALSE; 
}

