#pragma once

#include <bitset>
#include <cstdint>
#include <string>


namespace SettingsSettings {
	typedef unsigned int index_type;
	// indices of bitset
	enum class Index : index_type {SAVEDIRABS=0, VSYNC, SOFTWARERENDERER, DISPLAYFPS, PAUSEFOCUSLOST};
	constexpr char defaultDataDir[] = "data";
	constexpr char defaultSaveDir[] = "save";
	// default flag values
	constexpr bool fSaveDirAbs = false;
	constexpr bool fVsync = true;
	constexpr bool fSoftwareRenderer = false;
	constexpr bool fDisplayFPS = false;
	constexpr bool fPauseFocusLost = true;

	inline
	constexpr index_type toIndex(const Index i) {
		return static_cast<index_type>(i);
	}
}


// Read/process settings from ini file and command line
class Settings {
public:
	Settings(int, char**);
	~Settings() = default;
	bool getFlag(const SettingsSettings::Index) const;
	uint32_t rendererFlags(void) const;

	// absolute paths
	std::string rootPath;
	std::string dataPath;
	std::string savePath;
	std::bitset<8> flags;
	bool exitFlag = false;	// exit immediately after constructor?
};


inline
bool Settings::getFlag(const SettingsSettings::Index i) const {
	return flags.test(SettingsSettings::toIndex(i));
}
