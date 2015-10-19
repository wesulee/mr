#pragma once

#include "sdl_header.h"
#include <string>


// forward declarations

int msToTicks(const float, const int);
std::string q(const std::string&);
void shrinkRect(SDL_Rect&, const int);


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
	// default to permanent opaque
	AlphaFade();
	~AlphaFade() = default;
	bool update(void);
	void set(const Uint8, const Uint8, const unsigned int);
	Uint8 getAlpha(void) const;
private:
	Counter counter;
	float alphaf;
	float da;
	Uint8 alpha;
	Uint8 alphaEnd;
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
AlphaFade::AlphaFade() : counter(0), alphaf(alpha), da(0), alpha(SDL_ALPHA_OPAQUE), alphaEnd(alpha) {
}


inline
Uint8 AlphaFade::getAlpha() const {
	return alpha;
}
