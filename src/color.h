#pragma once

#include <cstdint>


#define COLOR_BLACK   Color{0, 0, 0}
#define COLOR_CYAN    Color{0, 255, 255}
#define COLOR_GREEN   Color{0, 255, 0}
#define COLOR_MAGENTA Color{255, 0, 255}
#define COLOR_RED     Color{255, 0 , 0}
#define COLOR_WHITE   Color{255, 255, 255}
#define COLOR_YELLOW  Color{255, 255, 0}


class Color {
public:
	// default color is magenta
	constexpr Color() : Color(COLOR_MAGENTA) {}
	constexpr Color(const uint8_t r, const uint8_t g, const uint8_t b) : R(r), G(g), B(b) {}
	constexpr Color(const Color&) = default;
	~Color() = default;
	Color& operator=(const Color&) = default;
	bool operator==(const Color& c) const {return ((R == c.R) && (G == c.G) && (B == c.B));}
	bool operator!=(const Color& c) const {return ((R != c.R) || (G != c.G) || (B != c.B));}

	uint8_t R;
	uint8_t G;
	uint8_t B;
};
