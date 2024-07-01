// Defines shared hook for game functions that are always active, regardless of config settings.

#include "pch.h"
#include "GameHooks.h"
#include "GameState.h"

#include "Cataclysm.h"

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
	CreateAndEnableHook(Functions::univUpdate, univUpdate, &orig_univUpdate);
	CreateAndEnableHook(Functions::opOptionsAccept, opOptionsAccept, &orig_opOptionsAccept);
	CreateAndEnableHook(Instructions::CheckPlayerWin_GameTypeCheck, CheckPlayerWin_GameTypeCheck, &orig_CheckPlayerWin_GameTypeCheck);
}

