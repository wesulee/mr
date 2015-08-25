#pragma once

#include "sdl_header.h"
#include <cmath>
#include <string>
#include <utility>


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


template<class T, std::size_t I>
static bool cmpPair(const T& a, const T& b) {
	return (std::get<I>(a) < std::get<I>(b));
}


inline
void copyDim(SDL_Surface* surf, SDL_Rect& r) {
	r.w = surf->w;
	r.h = surf->h;
}


inline
void shrinkRect(SDL_Rect& r, const int n) {
	r.x += n;
	r.y += n;
	r.w -= n * 2;
	r.h -= n * 2;
}


// center fg in bg
inline
void centerRect(const SDL_Rect& bg, SDL_Rect& fg) {
	fg.x = bg.x + ((bg.w - fg.w) / 2);
	fg.y = bg.y + ((bg.h - fg.h) / 2);
}


bool isBool(const std::string&);
bool string2bool(const std::string&);
bool isUInt(const std::string&);
bool isInt(const std::string&);
bool equivalent(const std::string&, const int);
// is v in [low, high] (INCLUSIVE)
template<typename T>
bool inRange(const T v, const T lo, const T hi) {
	return ((v >= lo) && (v <= hi));
}


int msToTicks(const float, const int);


// T should be a floating point type
template<typename T>
class Vector2D {
public:
	Vector2D() = default;
	Vector2D(const T a, const T b) : x(a), y(b) {}
	~Vector2D() = default;

	void normalize() {
		const T len = length();
		x /= len;
		y /= len;
	}

	T length() const {return std::sqrt(x * x + y * y);}

	Vector2D<T>& operator=(const Vector2D<T>& that) {
		x = that.x;
		y = that.y;
		return *this;
	}

	Vector2D<T>& operator+=(const Vector2D<T>& that) {
		x += that.x;
		y += that.y;
		return *this;
	}

	Vector2D<T>& operator*=(const T v) {
		x *= v;
		y *= v;
		return *this;
	}

	T x;
	T y;
};


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
	AlphaFade() : alpha(SDL_ALPHA_OPAQUE), alphaEnd(alpha), alphaf(alpha), da(0), counter(0) {}
	~AlphaFade() = default;
	bool update(void);
	void set(const Uint8, const Uint8, const unsigned int);
	Uint8 getAlpha() const {return alpha;}
private:
	Uint8 alpha;
	Uint8 alphaEnd;
	float alphaf;
	float da;
	Counter counter;
};


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
