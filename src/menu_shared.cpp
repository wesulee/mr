#include "menu_shared.h"
#include "font.h"
#include "game_data.h"
#include "save_helper.h"
#include "text_renderer.h"
#include "widget_scroll_bar.h"
#include "widget_text_list_view.h"
#include <algorithm>	// sort
#include <string>
#include <vector>


namespace MenuSettings {


void setup(TextListView& view) {
	view.setItemHeight(viewItemHeight);
	view.setScrollBarWidth(scrollBarWidth);
	view.setBackgroundColor(viewBg);
	view.setItemColors(viewItemOut, viewItemOver, viewItemDown);
	view.setScrollBarStyle({scrollBarBg, scrollBarOut, scrollBarOver, scrollBarDown});
	view.getTextRenderer()->setFont(Font{Font::DEFAULT_MONO, itemFontSize});
	view.getTextRenderer()->setColor(itemFontColor);
	std::vector<std::string> saves = SaveHelper::getUserSaveNames();
	std::sort(saves.begin(), saves.end());
	for (const auto& name : saves)
		view.add(name);
}


}	// namespace MenuSettings
