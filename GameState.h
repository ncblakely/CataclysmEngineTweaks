#pragma once

struct GameState
{
    std::optional<float> autosaveTimer = 0.0f;

    void ResetAutosaveTimer();
};

extern GameState g_GameState;