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

void InstallGameHooks(Assembler& assembler, Config& config)
{
	CreateAndEnableHook(Functions::univUpdate, univUpdate, &orig_univUpdate);
	CreateAndEnableHook(Functions::opOptionsAccept, opOptionsAccept, &orig_opOptionsAccept);
}

