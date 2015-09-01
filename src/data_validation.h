#pragma once


#ifndef NDEBUG
#include "utility.h"

#define IS_BOOL(a) isBool(a)
#define IS_UINT(a) isUInt(a)
#define IS_INT(a) isInt(a)
#define IN_RANGE(a, b, c) inRange(a, b, c)
#define EQUIVALENT(a, b) equivalent(a, b)
#else
#define IS_BOOL(a) true
#define IS_UINT(a) true
#define IS_INT(a) true
#define IN_RANGE(a, b, c) true
#define EQUIVALENT(a, b) true
#endif // NDEBUG
