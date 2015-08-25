#include "widget_data.h"
#include <cassert>


namespace WidgetDataSettings {
	constexpr int minButtonW = 60;	// minimum button dimensions
	constexpr int minButtonH = 30;
}


void WidgetData::init(TextRenderer* const tr) {
	using namespace WidgetDataSettings;
	assert(tr != nullptr);
	defaultTR = tr;
	//! TODO setup buttonDefault* correctly
	buttonDefaultW = minButtonW;
	buttonDefaultH = minButtonH;
}
