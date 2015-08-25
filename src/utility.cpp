#include "utility.h"
#include <boost/lexical_cast.hpp>
#include <cctype>	// isdigit
#include <cstdlib>	// atoi


bool isBool(const std::string& str) {
	return ((str == "true") || (str == "false"));
}


// returns false is str isn't bool ('true', 'false')
bool string2bool(const std::string& str) {
	return (str == "true");
}


// is str a valid unsigned integer?
bool isUInt(const std::string& str) {
	if (str.empty())
		return false;
	for (auto it = str.cbegin(); it != str.cend(); ++it) {
		if (!std::isdigit(*it))
			return false;
	}
	return true;
}


bool isInt(const std::string& str) {
	if (str.empty())
		return false;
	auto it = str.cbegin();
	if (!std::isdigit(*it)) {
		if ((*it) != '-')
			return false;	// str begins with non-numeric character that isn't '-'
		else if (str.size() <= 1)
			return false;	// str is '-'
		++it;
	}
	for (; it != str.cend(); ++it) {
		if (!std::isdigit(*it))
			return false;
	}
	return true;
}


bool equivalent(const std::string& str, const int i) {
	return boost::lexical_cast<std::string>(i) == str;
}


int msToTicks(const float frame, const int ms) {
	return std::ceil(static_cast<float>(ms) / frame);
}


bool AlphaFade::update() {
	if (counter.finished()) return true;
	counter.increment();
	if (counter.finished()) {
		alpha = alphaEnd;
	}
	else {
		alphaf += da;
		alpha = static_cast<Uint8>(alphaf);
	}
	return false;
}


void AlphaFade::set(const Uint8 start, const Uint8 end, const unsigned int ticks) {
	alpha = start;
	alphaEnd = end;
	alphaf = start;
	da = -(static_cast<int>(start) - end) / static_cast<float>(ticks);
	counter.reset();
	counter.setMaxTicks(ticks);
}
