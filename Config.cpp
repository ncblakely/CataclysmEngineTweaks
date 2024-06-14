#include "pch.h"
#include "Config.h"
#include "INIReader.h"

Config Config::FromFile(const char* path)
{
	Config config{};
	INIReader reader{ path };

	////////////////////////////////////////////////////////////////
	// Engine section

	config.m_updateRateShift = (udword)std::max(0l, reader.GetInteger("Engine", "UpdateRateShift", 0));
	config.m_updateRate = (float)(16 << config.m_updateRateShift);
	
	////////////////////////////////////////////////////////////////
	// Display section

	config.m_highDetailMode = reader.GetBoolean("Display", "HighDetailMode", false);
	config.m_newResolutionPicker = reader.GetBoolean("Display", "NewResolutionPicker", false);
	config.m_borderlessWindow = reader.GetBoolean("Display", "BorderlessWindow", false);
	config.m_disableIntroMovies = reader.GetBoolean("Display", "DisableIntroMovies", false);

	////////////////////////////////////////////////////////////////
	// Game section

	config.m_autoSaveInterval = (udword)std::max(0l, reader.GetInteger("Game", "AutoSaveInterval", 0));

	////////////////////////////////////////////////////////////////
	// OpenGL section

	config.m_openGLFix = reader.GetBoolean("OpenGL", "FixWindowsNTBug", true);
	config.m_safeGL = reader.GetBoolean("OpenGL", "SafeGL", false);

	return config;
}
