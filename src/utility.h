#pragma once

#include "constants.h"
#include "sdl_header.h"
#include <cassert>
#include <cmath>
#include <string>


// forward declarations

int msToTicks(const float, const int);
std::string q(const std::string&);
void shrinkRect(SDL_Rect&, const int);
Constants::float_type correctFloat(const Constants::float_type);


template<typename T>
T square(const T v) {
	return v * v;
}


// hashing enum class workaround
// http://stackoverflow.com/a/24847480
struct EnumClassHash {
	template<typename T>
	std::size_t operator()(T t) const {
		return static_cast<std::size_t>(t);
	}
};


// is v in [low, high] (INCLUSIVE)
template<typename T>
bool inRange(const T v, const T lo, const T hi) {
	return ((v >= lo) && (v <= hi));
}


class Counter {
public:
	Counter();
	Counter(const unsigned int);
	~Counter() = default;
	void increment(void);
	void reset(void);
	unsigned int getTicks(void) const;
	void setTicks(const unsigned int);
	bool finished(void) const;
	unsigned int getMaxTicks(void) const;
	void setMaxTicks(const unsigned int);
	Counter& operator++(void);
private:
	unsigned int ticks = 0;
	unsigned int maxTicks;
};


class AlphaFade {
public:
	AlphaFade() = default;
	~AlphaFade() = default;
	bool update(const Constants::float_type);
	void set(const Constants::float_type, const Uint8, const Uint8);
	Uint8 getAlpha(void) const;
private:
	float alpha;	// current alpha
	float dAlpha;	// change in alpha per second
	float timeRem;	// remaining time
};


// definitions

inline
std::string q(const std::string& str) {
	return (std::string{'\"'} + str + '\"');
}


inline
void shrinkRect(SDL_Rect& r, const int n) {
	r.x += n;
	r.y += n;
	r.w -= n * 2;
	r.h -= n * 2;
}


// values that are just off from whole number when casted to int are corrected
// returns argument if no correction made
inline
Constants::float_type correctFloat(const Constants::float_type v) {
	assert(!std::isnan(v));
	Constants::float_type intPart;	// dummy
	const Constants::float_type fracPart = std::modf(v, &intPart);
	if (std::abs(fracPart) >= (static_cast<Constants::float_type>(1) - Constants::floatInc)) {
		return (v + (fracPart * static_cast<Constants::float_type>(1.1)));
	}
	return v;
}


inline
Counter::Counter() : maxTicks(0) {
}


inline
Counter::Counter(const unsigned int max) : maxTicks(max) {
}


inline
void Counter::increment() {
	++ticks;
}


inline
void Counter::reset() {
	ticks = 0;
}


inline
unsigned int Counter::getTicks() const {
	return ticks;
}


inline
void Counter::setTicks(const unsigned int t) {
	ticks = t;
}


inline
bool Counter::finished() const {
	return (ticks >= maxTicks);
}


inline
unsigned int Counter::getMaxTicks() const {
	return maxTicks;
}


inline
void Counter::setMaxTicks(const unsigned int t) {
	maxTicks = t;
}


inline
Counter& Counter::operator++() {
	++ticks;
	return *this;
}


inline
Uint8 AlphaFade::getAlpha() const {
	return static_cast<Uint8>(alpha);
}
