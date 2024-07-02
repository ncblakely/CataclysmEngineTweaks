#include "pch.h"
#include "Config.h"
#include "INIReader.h"

static constexpr long MaxUpdateRateShift = 3;

Config Config::FromFile(const char* path)
{
	Config config{};
	INIReader reader{ path };

	////////////////////////////////////////////////////////////////
	// Engine section

	config.UpdateRateShift = (udword)Clamp(0l, MaxUpdateRateShift, reader.GetInteger("Engine", "UpdateRateShift", 0));
	config.UpdateRate = (float)(16 << config.UpdateRateShift);
	config.DisableFramerateLimit = reader.GetBoolean("Engine", "DisableFramerateLimit", false);
	
	////////////////////////////////////////////////////////////////
	// Display section

	config.HighDetailMode = reader.GetBoolean("Display", "HighDetailMode", false);
	config.NewRendererSelection = reader.GetBoolean("Display", "NewRendererSelection", false);
	config.BorderlessWindow = reader.GetBoolean("Display", "BorderlessWindow", false);
	config.DisableIntroMovies = reader.GetBoolean("Display", "DisableIntroMovies", false);
	config.UnlimitedVideoMemory = reader.GetBoolean("Display", "UnlimitedVideoMemory", false);

	////////////////////////////////////////////////////////////////
	// Game section

	config.AutoSaveInterval = (udword)std::max(0l, reader.GetInteger("Game", "AutoSaveInterval", 0));

	////////////////////////////////////////////////////////////////
	// OpenGL section

	config.OpenGLFix = reader.GetBoolean("OpenGL", "FixWindowsNTBug", true);
	config.NoCompiledVertexArrays = reader.GetBoolean("OpenGL", "NoCompiledVertexArrays", false);

	////////////////////////////////////////////////////////////////
	// Audio section
	config.ForceWaveOut = reader.GetBoolean("Audio", "ForceWaveOut", false);
	config.DisableEqualizer = reader.GetBoolean("Audio", "DisableEqualizer", false);

	return config;
}
