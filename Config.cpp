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

	config.m_updateRateShift = (udword)Clamp(0l, MaxUpdateRateShift, reader.GetInteger("Engine", "UpdateRateShift", 0));
	config.m_updateRate = (float)(16 << config.m_updateRateShift);
	config.m_disableFramerateLimit = reader.GetBoolean("Engine", "DisableFramerateLimit", false);
	
	////////////////////////////////////////////////////////////////
	// Display section

	config.m_highDetailMode = reader.GetBoolean("Display", "HighDetailMode", false);
	config.m_newRendererSelection = reader.GetBoolean("Display", "NewRendererSelection", false);
	config.m_borderlessWindow = reader.GetBoolean("Display", "BorderlessWindow", false);
	config.m_disableIntroMovies = reader.GetBoolean("Display", "DisableIntroMovies", false);
	config.m_unlimitedVideoMemory = reader.GetBoolean("Display", "UnlimitedVideoMemory", false);

	////////////////////////////////////////////////////////////////
	// Game section

	config.m_autoSaveInterval = (udword)std::max(0l, reader.GetInteger("Game", "AutoSaveInterval", 0));

	////////////////////////////////////////////////////////////////
	// OpenGL section

	config.m_openGLFix = reader.GetBoolean("OpenGL", "FixWindowsNTBug", true);
	config.m_safeGL = reader.GetBoolean("OpenGL", "SafeGL", false);

	// Debug options
	config.m_noPalettes = reader.GetBoolean("Debug", "NoPalettes", false);
	config.m_noVertexArrays = reader.GetBoolean("Debug", "NoVertexArrays", false);
	config.m_noCompiledVertexArrays = reader.GetBoolean("Debug", "NoCompiledVertexArrays", false);
	config.m_noPointSmooth = reader.GetBoolean("Debug", "NoPointSmooth", false);
	config.m_noLineSmooth = reader.GetBoolean("Debug", "NoLineSmooth", false);
	config.m_disableEqualizer = reader.GetBoolean("Debug", "DisableEqualizer", false);
	config.m_disablePitchShift = reader.GetBoolean("Debug", "DisablePitchShift", false);
	config.m_disableVolPan = reader.GetBoolean("Debug", "DisableVolPan", false);

	return config;
}
