#pragma once

#include <bitset>


// Settings for game that are needed throughout runtime
class GameSettings {
	typedef std::size_t index_type;
public:
	enum class Index : index_type {PAUSEFOCUSLOST=0};
	void set(const Index, const bool=true);
	bool test(const Index) const;
private:
	std::bitset<1> flags;
};


inline
void GameSettings::set(const Index i, const bool b) {
	flags.set(static_cast<index_type>(i), b);
}


inline
bool GameSettings::test(const Index i) const {
	return flags.test(static_cast<index_type>(i));
}
