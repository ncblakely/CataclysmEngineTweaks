#pragma once

struct Config
{
	static Config FromFile(const char* path);

	// Get the universe update rate in frames per second (e.g 64 FPS)
	// #define UNIVERSE_UPDATE_RATE        16
	real32 GetUniverseUpdateRate() const
	{
		return (real32)UpdateRate;
	}

	// Get the adjustment factor for the universe update rate
	udword GetUniverseUpdateRateFactor() const {
		return (1 << UpdateRateShift);
	}

	udword GetUniverseUpdateRateShift() const {
		return UpdateRateShift;
	}

	// Get the inverse update rate
	// #define UNIVERSE_UPDATE_PERIOD      (1.0f/(real32)UNIVERSE_UPDATE_RATE)
	real32 GetUniverseUpdatePeriod() const
	{
		return 1.0f / UpdateRate;
	}

	// Get the resolution of the internal task timer
	udword GetTimerResolutionMax() const
	{
		return 15 * (udword)(GetUniverseUpdateRate());
	}

	real32 UpdateRate = 0;
	udword UpdateRateShift = 0;
	bool HighDetailMode = true;
	bool UnlimitedVideoMemory = true;
	bool NewRendererSelection = true;
	bool BorderlessWindow = true;
	bool DisableIntroMovies = true;
	udword AutoSaveInterval = 5;
	bool OpenGLFix = true;
	bool DisableFramerateLimit = false;
	bool NoCompiledVertexArrays = false;
	bool ForceWaveOut = false;
	bool DisableEqualizer = false;
};

extern Config g_Config;