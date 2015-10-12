#include "settings.h"
#include "console.h"
#include "constants.h"
#include "game_data.h"	// exitCode
#include "ini_parser.h"
#include "logger.h"
#include "sdl_helper.h"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <sstream>
#define BOOST_FILESYSTEM_NO_DEPRECATED


namespace fs = boost::filesystem;
namespace po = boost::program_options;


namespace SettingsHelper {
// forward declarations
struct SettingsException;
bool operator==(const SDL_version&, const SDL_version&);
std::ostream& operator<<(std::ostream&, const SDL_version&);
boost::optional<bool> getValue(const std::string&);
void setBasePath(std::string&);
void setupArguments(const int, char**, po::variables_map&);
void procImmediateArgs(po::variables_map&);
void procIni(Settings&);
void procArguments(po::variables_map&, Settings&);
void printAbout(std::ostream&);
void printDrivers(std::ostream&);
void printSDLError(std::ostream&);


template<class T>
void setFlag(const IniParser::ValueMap& map, const char* name, T& bitset, std::size_t index) {
	auto find = map.find(name);
	if (find != map.end()) {
		auto val = getValue(find->second);
		if (val)
			bitset.set(index, *val);
	}
}


// notify that something exceptional happened, not necessarily an error
struct SettingsException {
	bool errorFlag;
};


bool operator==(const SDL_version& v1, const SDL_version& v2) {
	return (
		(v1.major == v2.major)
		&& (v1.minor == v2.minor)
		&& (v1.patch == v2.patch)
	);
}


std::ostream& operator<<(std::ostream& os, const SDL_version& v) {
	os << +v.major << '.' << +v.minor << '.' << +v.patch;
	return os;
}


boost::optional<bool> getValue(const std::string& str) {
	if (str.empty())
		return boost::none;
	int value;
	try {
		value = std::stoi(str);
	}
	catch (std::invalid_argument const& e) {
		return boost::none;
	}
	if (value == 0)
		return false;
	else
		return true;
}


void setBasePath(std::string& path) {
	char* root = SDL_GetBasePath();
	if (root == nullptr) {
		// in this case, log file may not be in same directory as executable
		Logger::instance().setPath(Constants::loggerFName);
		SDL::logError("SDL_GetBasePath");
		throw SettingsException{true};
	}
	path = root;
	SDL_free(root);
	Logger::instance().setPath(path + Constants::loggerFName);
}


void setupArguments(const int argc, char** argv, po::variables_map& vm) {
	po::options_description desc{"Options"};
	desc.add_options()
		("help", "print usage")
		("about", "information about this application")
		("drivers", "display available 2D rendering drivers")
		("savedir", po::value<std::string>(), "set save directory")
	;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
	}
	catch (po::error& e) {
		std::stringstream ss;
		std::cerr << "Error processing arguments: " << e.what() << std::endl << std::endl << desc;
		ss << e.what() << std::endl << desc;
		Logger::instance().log(ss.str());
		throw SettingsException{true};
	}
	// if help argument given, process now
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		throw SettingsException{false};
	}
}


void procImmediateArgs(po::variables_map& vm) {
	if (vm.count("about")) {
		printAbout(std::cout);
		throw SettingsException{false};
	}
	if (vm.count("drivers")) {
		printDrivers(std::cout);
		throw SettingsException{false};
	}
}


void procIni(Settings& settings) {
	using namespace SettingsSettings;
	IniParser::ValueMap iniMap = IniParser::readFile(settings.rootPath + Constants::iniFileName);
	auto it = iniMap.find("DataDir");
	if (it != iniMap.end())
		settings.dataPath = fs::absolute(it->second, settings.rootPath).string();
	else
		settings.dataPath = fs::absolute(defaultDataDir, settings.rootPath).string();
	it = iniMap.find("SaveDir");
	if (it != iniMap.end())
		settings.savePath = fs::absolute(it->second, settings.rootPath).string();
	else
		settings.savePath = fs::absolute(defaultSaveDir, settings.rootPath).string();
	it = iniMap.find("Renderer");
	if (it != iniMap.end())
		settings.renderer = it->second;
	setFlag(iniMap, "Vsync", settings.flags, toIndex(Index::VSYNC));
	setFlag(iniMap, "DisplayFPS", settings.flags, toIndex(Index::DISPLAYFPS));
	setFlag(iniMap, "PauseFocusLost", settings.flags, toIndex(Index::PAUSEFOCUSLOST));
}


void procArguments(po::variables_map& vm, Settings& settings) {
	using namespace SettingsSettings;
	if (vm.count("savedir")) {
		// arguments provided by command-line override ini file
		settings.savePath = fs::absolute(vm["savedir"].as<std::string>()).string();
	}
}


void printAbout(std::ostream& os) {
	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	const SDL_version* linkedTTF = TTF_Linked_Version();
	os << "mr" << std::endl
	   << "Libraries:" << std::endl;
	// print SDL
	os << '\t' << "SDL ";
	if (compiled == linked)
		os << compiled;
	else
		os << "compiled: " << compiled << " linked: " << linked;
	os << std::endl;
	// print SDL_ttf
	SDL_TTF_VERSION(&compiled);
	os << '\t' << "SDL_ttf ";
	if (compiled == *linkedTTF)
		os << compiled;
	else
		os << "compiled: " << compiled << " linked: " << *linkedTTF;
	os << std::endl;
	// print Boost
	os << '\t' << "Boost " << BOOST_VERSION / 100000 << '.' << BOOST_VERSION / 100 % 1000
	   << '.' << BOOST_VERSION % 100 << std::endl;
}


void printDrivers(std::ostream& os) {
	using namespace SettingsHelper;
	int available = SDL_GetNumRenderDrivers();
	if (available == 0) {
		printSDLError(os);
		return;
	}
	SDL_RendererInfo info;
	std::string flags;
	for (int i = 0; i < available; ++i) {
		if (SDL_GetRenderDriverInfo(i, &info) != 0) {
			printSDLError(os);
			return;
		}
		flags = SDL::rendererFlagsToString(info.flags);
		if (flags.empty())
			flags = "none";
		os << "index: " << i << " name: \"" << info.name << "\" flags: " << flags << std::endl;
	}
}


void printSDLError(std::ostream& os) {
	os << "An error has occurred: " << SDL_GetError() << std::endl;
}

}	// namespace SettingsHelper


#define SET_FLAG(vec,i,v) if (v) vec.set(i)


// Note: sets Logger path since it is a priority to set it before any errors occur
// Should try to catch exceptions here and set exitFlag when appropriate
Settings::Settings(int argc, char** argv) {
	using namespace SettingsSettings;
	using namespace SettingsHelper;
	Console::startTimer();
	SET_FLAG(flags, toIndex(Index::VSYNC), fVsync);
	SET_FLAG(flags, toIndex(Index::DISPLAYFPS), fDisplayFPS);
	SET_FLAG(flags, toIndex(Index::PAUSEFOCUSLOST), fPauseFocusLost);
	po::variables_map vm;
	try {
		// set rootPath and Logger path
		setBasePath(rootPath);
		// process command-line arguments
		setupArguments(argc, argv, vm);
		procImmediateArgs(vm);
		// process ini file
		procIni(*this);
	}
	catch (SettingsException const& e) {
		if (e.errorFlag)
			GameData::instance().exitCode = 1;
		exitFlag = true;
		return;
	}
	// finish processing arguments
	procArguments(vm, *this);
}


std::pair<std::string, bool> Settings::getRendererPref() const {
	using namespace SettingsSettings;
	std::pair<std::string, bool> ret;
	ret.first = renderer;
	ret.second = flags.test(toIndex(Index::VSYNC));
	return ret;
}
