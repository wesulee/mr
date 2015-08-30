#include "console.h"
#include "constants.h"
#include <iomanip>


std::chrono::steady_clock::time_point  Console::startTime = std::chrono::steady_clock::time_point{};
std::ostream& Console::os = std::cout;


std::ostream& Console::begin() {
	// print format: minutes:seconds.milliseconds
	std::chrono::milliseconds runtime = std::chrono::duration_cast<decltype(runtime)>(
		std::chrono::steady_clock::now() - startTime
	);
	const auto runtimeMS = runtime.count();
	const auto minutes = (runtimeMS / (1000 * 60));	// total minutes, rounded down
	const auto seconds = ((runtimeMS - (minutes * (60 * 1000))) / 1000);	// remaining seconds, rounded down
	const auto milliseconds = (runtimeMS - (seconds * 1000) - (minutes * 60 * 1000));		// remaining milliseconds
	os << '[' << minutes << ':' << std::setfill('0') << std::setw(2) << seconds;
	if (Constants::ConsoleMilliseconds)
		os << '.' << std::setfill('0') << std::setw(3) << milliseconds;
	os << ']' << ' ';
	return os;
}


void Console::startTimer() {
	startTime = std::chrono::steady_clock::now();
}
