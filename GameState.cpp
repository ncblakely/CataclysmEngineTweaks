#include "pch.h"
#include "GameState.h"

GameState g_GameState;

void GameState::ResetAutosaveTimer()
{
	udword autosaveInterval = g_Config.GetAutosaveInterval();
	if (autosaveInterval > 0)
	{
		this->autosaveTimer = autosaveInterval * 60.0f;
	}
	else
	{
		this->autosaveTimer = {};
	}
}