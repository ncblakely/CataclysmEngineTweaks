#pragma once

class Config
{
public:
	static Config FromFile(const char* path);

	// Get the universe update rate in frames per second (e.g 64 FPS)
	// #define UNIVERSE_UPDATE_RATE        16
	real32 GetUniverseUpdateRate() const
	{
		return (real32)m_updateRate;
	}

	// Get the adjustment factor for the universe update rate
	udword GetUniverseUpdateRateFactor() const {
		return (1 << m_updateRateShift);
	}

	udword GetUniverseUpdateRateShift() const {
		return m_updateRateShift;
	}

	// Get the inverse update rate
	// #define UNIVERSE_UPDATE_PERIOD      (1.0f/(real32)UNIVERSE_UPDATE_RATE)
	real32 GetUniverseUpdatePeriod() const
	{
		return 1.0f / m_updateRate;
	}

	// Get the resolution of the internal task timer
	udword GetTimerResolutionMax() const
	{
		return 15 * (udword)(GetUniverseUpdateRate());
	}

	bool IsHighDetailModeEnabled() const
	{
		return m_highDetailMode;
	}

	bool IsNewResolutionPickerEnabled() const
	{
		return m_newResolutionPicker;
	}

	bool IsBorderlessWindowEnabled() const
	{
		return m_borderlessWindow;
	}

	bool IsIntroMovieDisabled() const
	{
		return m_disableIntroMovies;
	}

	udword GetAutosaveInterval() const
	{
		return m_autoSaveInterval;
	}

	bool OpenGLFix() const
	{
		return m_openGLFix;
	}

private:
	real32 m_updateRate = 0;
	udword m_updateRateShift = 0;
	bool m_highDetailMode = false;
	bool m_newResolutionPicker = false;
	bool m_borderlessWindow = false;
	bool m_disableIntroMovies = false;
	udword m_autoSaveInterval = 0;
	bool m_openGLFix = false;
};

extern Config g_Config;