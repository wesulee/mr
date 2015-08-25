#include "settings.h"
#include "constants.h"
#include "game_data.h"	// exitCode
#include "ini_parser.h"
#include "logger.h"
#include "sdl_helper.h"
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <sstream>


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
void finalizePaths(Settings&);
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


// implementation


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
		SDL::logError("SDL_GetBasePath");
		throw SettingsException{true};
	}
	path = root;
	SDL_free(root);
	Logger::setPath(path + Logger::getPath());
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
	auto find = iniMap.find("DataDir");
	if (find != iniMap.end())
		settings.dataPath = find->second;
	else
		settings.dataPath = defaultDataDir;
	find = iniMap.find("SaveDir");
	if (find != iniMap.end())
		settings.savePath = find->second;
	setFlag(iniMap, "SaveDirAbs", settings.flags, toIndex(Index::SAVEDIRABS));
	setFlag(iniMap, "Vsync", settings.flags, toIndex(Index::VSYNC));
	setFlag(iniMap, "SoftwareRenderer", settings.flags, toIndex(Index::SOFTWARERENDERER));
	setFlag(iniMap, "DisplayFPS", settings.flags, toIndex(Index::DISPLAYFPS));
	setFlag(iniMap, "PauseFocusLost", settings.flags, toIndex(Index::PAUSEFOCUSLOST));
}


void procArguments(po::variables_map& vm, Settings& settings) {
	using namespace SettingsSettings;
	if (vm.count("vsync")) {
		settings.flags.set(toIndex(Index::SOFTWARERENDERER), vm["vsync"].as<bool>());
	}
	// NOTE: savedir argument is treated as absolute path
	if (vm.count("savedir")) {
		settings.flags.set(toIndex(Index::SAVEDIRABS));
		settings.savePath = vm["savedir"].as<std::string>();
	}
}


void finalizePaths(Settings& settings) {
	using namespace SettingsSettings;
	// make data absolute path
	if (settings.dataPath.empty())	// data path never set
		settings.dataPath = settings.rootPath + defaultDataDir;	// default data path
	else
		settings.dataPath = settings.rootPath + settings.dataPath;
	// if not savePath not absolute, make path absolute
	if (!settings.flags.test(toIndex(Index::SAVEDIRABS))) {
		if (settings.savePath.empty())
			settings.savePath = settings.rootPath + defaultSaveDir;	// default save path
		else
			settings.savePath = settings.rootPath + settings.savePath;
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
	for (int i = 0; i < available; ++i) {
		if (SDL_GetRenderDriverInfo(i, &info) != 0) {
			printSDLError(os);
			return;
		}
		os << "Index: " << i << " name: " << info.name;
		if (info.flags != 0) {
			os << " flags:";
			if (info.flags & SDL_RENDERER_SOFTWARE)
				os << " SOFTWARE";
			if (info.flags & SDL_RENDERER_ACCELERATED)
				os << " ACCELERATED";
			if (info.flags & SDL_RENDERER_PRESENTVSYNC)
				os << " PRESENTVSYNC";
			if (info.flags & SDL_RENDERER_TARGETTEXTURE)
				os << " TARGETTEXTURE";
		}
		os << std::endl;
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
	SET_FLAG(flags, toIndex(Index::SAVEDIRABS), fSaveDirAbs);
	SET_FLAG(flags, toIndex(Index::VSYNC), fVsync);
	SET_FLAG(flags, toIndex(Index::SOFTWARERENDERER), fSoftwareRenderer);
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
	finalizePaths(*this);
}


uint32_t Settings::rendererFlags() const {
	using namespace SettingsSettings;
	uint32_t f = 0;
	if (flags.test(toIndex(Index::VSYNC)))
		f |= SDL_RENDERER_PRESENTVSYNC;
	if (flags.test(toIndex(Index::SOFTWARERENDERER)))
		f |= SDL_RENDERER_SOFTWARE;
	else
		f |= SDL_RENDERER_ACCELERATED;
	return f;
}
