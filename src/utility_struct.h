#pragma once

#include "constants.h"	// float_type
#include <cmath>	// sqrt


struct IntPair {
	IntPair() = default;
	IntPair(const IntPair&) = default;
	IntPair(const int, const int);
	~IntPair() = default;
	void set(const int, const int);
	IntPair& operator=(const IntPair&) = default;
	IntPair& operator+=(const IntPair&);
	bool operator==(const IntPair&);
	bool operator!=(const IntPair&);

	int first;
	int second;
};


// T should be a floating point type
template<typename T = Constants::float_type>
class Vector2D {
public:
	typedef T underlying_type;

	Vector2D() = default;
	Vector2D(const T a, const T b);
	~Vector2D() = default;
	void normalize(void);
	T length(void) const;
	Vector2D<T>& operator=(const Vector2D<T>&);
	Vector2D<T>& operator+=(const Vector2D<T>&);
	Vector2D<T>& operator-=(const Vector2D<T>&);
	Vector2D<T>& operator*=(const T);
	Vector2D<T>& operator/=(const T);
	Vector2D<T> operator+(const Vector2D<T>&) const;
	Vector2D<T> operator-(const Vector2D<T>&) const;
	Vector2D<T> operator*(const T) const;
	Vector2D<T> operator/(const T) const;

	T x;
	T y;
};


inline
IntPair::IntPair(const int a, const int b) : first(a), second(b) {
}


inline
void IntPair::set(const int a, const int b) {
	first = a;
	second = b;
}


inline
IntPair& IntPair::operator+=(const IntPair& that) {
	first += that.first;
	second += that.second;
	return *this;
}


inline
bool IntPair::operator==(const IntPair& that) {
	return ((first == that.first) && (second == that.second));
}


inline
bool IntPair::operator!=(const IntPair& that) {
	return ((first != that.first) || (second != that.second));
}


template<class T>
Vector2D<T>::Vector2D(const T a, const T b) : x(a), y(b) {
}


template<class T>
void Vector2D<T>::normalize() {
	const T len = length();
	x /= len;
	y /= len;
}


template<class T>
T Vector2D<T>::length() const {
	return std::sqrt((x * x) + (y * y));
}


template<class T>
Vector2D<T>& Vector2D<T>::operator=(const Vector2D<T>& that) {
	x = that.x;
	y = that.y;
	return *this;
}


template<class T>
Vector2D<T>& Vector2D<T>::operator+=(const Vector2D<T>& that) {
	x += that.x;
	y += that.y;
	return *this;
}


template<class T>
Vector2D<T>& Vector2D<T>::operator-=(const Vector2D<T>& that) {
	x -= that.x;
	y -= that.y;
	return *this;
}


template<class T>
Vector2D<T>& Vector2D<T>::operator*=(const T v) {
	x *= v;
	y *= v;
	return *this;
}


template<class T>
Vector2D<T>& Vector2D<T>::operator/=(const T v) {
	x /= v;
	y /= v;
	return *this;
}


template<class T>
Vector2D<T> Vector2D<T>::operator+(const Vector2D<T>& that) const {
	return Vector2D<T>{x + that.x, y + that.y};
}


template<class T>
Vector2D<T> Vector2D<T>::operator-(const Vector2D<T>& that) const {
	return Vector2D<T>{x - that.x, y - that.y};
}


template<class T>
Vector2D<T> Vector2D<T>::operator*(const T v) const {
	return Vector2D<T>{x * v, y * v};
}


template<class T>
Vector2D<T> Vector2D<T>::operator/(const T v) const {
	return Vector2D<T>{x / v, y / v};
}
