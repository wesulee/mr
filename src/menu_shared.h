#pragma once

#include "color.h"


class TextListView;


// load/save menu
namespace MenuSettings {
	constexpr Color viewBg = COLOR_WHITE;
	constexpr Color viewItemOut{153, 204, 255};
	constexpr Color viewItemOver{51, 153, 255};
	constexpr Color viewItemDown{41, 122, 204};
	constexpr Color scrollBarBg{114, 125, 142};
	constexpr Color scrollBarOut{57, 62, 71};
	constexpr Color scrollBarOver{39, 43, 49};
	constexpr Color scrollBarDown{19, 21, 24};
	constexpr Color itemFontColor = COLOR_BLACK;
	constexpr int viewPadding = 50;
	constexpr int viewItemHeight = 35;
	constexpr int scrollBarWidth = 15;
	constexpr int itemFontSize = 16;

	void setup(TextListView&);
}
