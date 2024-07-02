#include "pch.h"
#include "GameState.h"

GameState g_GameState;

void GameState::ResetAutosaveTimer()
{
	if (g_Config.AutoSaveInterval > 0)
	{
		this->autosaveTimer = g_Config.AutoSaveInterval * 60.0f;
	}
	else
	{
		this->autosaveTimer = {};
	}
}