#include "widget_text_list_view.h"
#include "canvas.h"
#include "constants.h"
#include "utility.h"	// inRange
#include "widget_event.h"
#include "widget_text_item.h"
#include <algorithm>
#include <cassert>


static void insert(std::vector<DrawRange>& vec, const std::size_t index, const WidgetState s) {
	DrawRange dr;
	dr.lo = index;
	dr.hi = index;
	dr.state = s;
	// find pos that contains index
	auto it = vec.begin();
	while (it != vec.end()) {
		if ((index >= it->lo) && (index <= it->hi))
			break;
		++it;
	}
	assert(it != vec.end());
	// check endpoints
	if (it->lo == it->hi) {
		it->state = s;
		return;
	}
	if (index == it->lo) {
		++(it->lo);
		vec.insert(it, dr);
	}
	else if (index == it->hi) {
		--(it->hi);
		++it;
		vec.insert(it, dr);
	}
	else {
		assert((it->hi - it->lo) >= 2);
		DrawRange tmpDr = *it;
		tmpDr.hi = index - 1;
		it->lo = index + 1;
		it = vec.insert(it, dr);
		vec.insert(it, tmpDr);
	}
}


TextListView::TextListView() {
	using std::placeholders::_1;
	bar.setCallback(std::bind(&TextListView::scrollBarCallback, this, _1));
	bar._resize(IntPair{10, bounds.h}, WidgetResizeFlag::PARENT);
	updateScrollBarPos();
	drawItems.reserve(5);	// max size
	visibleStart = Constants::maxIndex;
	overItem = Constants::maxIndex;
	downItem = Constants::maxIndex;
}


TextListView::~TextListView() {
	for (auto item : items)
		delete item;
}


void TextListView::add(const std::string& text) {
	assert(bar.getContentOffset() == 0);
	TextItem* item = new TextItem;
	item->_setParent(this);
	item->_setIndex(items.size());
	item->_setPos(IntPair{0, static_cast<int>(items.size()) * itemHeight});
	item->_resize(IntPair{getItemWidth(), itemHeight}, WidgetResizeFlag::PARENT);
	item->setText(text);
	items.push_back(item);
	bar.setContentSize(static_cast<int>(items.size()) * itemHeight);
	updateVisible();
	updateDrawRange();
}


void TextListView::setItemHeight(const int h) {
	assert(h > 0);
	assert(items.empty());
	itemHeight = h;
}


void TextListView::setScrollBarWidth(const int w) {
	assert(w < bounds.w);
	bar._resize(IntPair{w, bar.getSize().second}, WidgetResizeFlag::PARENT);
	bar._setPos(IntPair{bounds.w - w, bar.getPos().second});
}


// background colors
void TextListView::setItemColors(const Color& out, const Color& over, const Color& down) {
	colItemBgOut = out;
	colItemBgOver = over;
	colItemBgDown = down;
}


void TextListView::draw(Canvas& can) {
	can.setColor(colBg);
	can.fillRect(0, 0, bounds.w, bounds.h);
	can.setOffset(0, -bar.getContentOffset());
	for (const auto& di : drawItems) {
		switch (di.state) {
		case WidgetState::OUT:
			can.setColor(colItemBgOut);
			break;
		case WidgetState::OVER:
			can.setColor(colItemBgOver);
			break;
		case WidgetState::DOWN:
			can.setColor(colItemBgDown);
			break;
		}
		for (std::size_t i = di.lo; i <= di.hi; ++i) {
			can.fillRect(items[i]->getBounds());
			items[i]->draw(can);
		}
	}
	can.clearOffset();
	can.setRelViewport(bar.getBounds());
	bar.draw(can);
	can.clearViewport();
}


void TextListView::event(WidgetEvent& e) {
	WEMouseAutoUpdate mTest;
	mTest.set(e, bounds);
	bar.event(e);
	switch (e.type) {
	case WidgetEventType::MOUSE_MOVE:
		if (mousePressed || barPressed)
			break;
		if (!mTest.inBounds()) {
			overItem = Constants::maxIndex;
			updateDrawRange();
			break;
		}
		if ((e.move.x >= 0) && (e.move.x < bar.getPos().first))
			overItem = getItemY(e.move.y);
		else
			overItem = Constants::maxIndex;
		updateDrawRange();
		break;
	case WidgetEventType::MOUSE_PRESS:
		mousePressed = true;
		barPressed = false;		// default value
		overItem = Constants::maxIndex;
		if (e.press.x < 0) {
			setDownItem(Constants::maxIndex);
			updateDrawRange();
			break;
		}
		if (e.press.x < bar.getPos().first) {
			if (mTest.inBounds())
				setDownItem(getItemY(e.press.y));
			else
				setDownItem(Constants::maxIndex);
			updateDrawRange();
		}
		else {
			barPressed = true;
		}
		break;
	case WidgetEventType::MOUSE_RELEASE:
		mousePressed = false;
		barPressed = false;
		break;
	default:
		break;
	}
}


IntPair TextListView::getPrefSize() const {
	return IntPair{Constants::WSizeExpand, Constants::WSizeExpand};
}


IntPair TextListView::getMinSize() const {
	return IntPair{100, 100};	//! ?
}


void TextListView::_resize(const IntPair& p, const WidgetResizeFlag) {
	assert((p.first > 0) && (p.second > 0));
	assert(bar.getContentOffset() == 0);
	const int prevItemWidth = getItemWidth();
	bounds.w = p.first;
	bounds.h = p.second;
	updateScrollBarPos();
	if (items.empty())
		return;
	reset();
	if (getItemWidth() != prevItemWidth) {
		IntPair resSz{getItemWidth(), itemHeight};
		for (auto item : items) {
			item->_resize(resSz, WidgetResizeFlag::PARENT);
			assert(item->getSize() == resSz);
		}
	}
	updateVisible();
	updateDrawRange();
}


//! TODO limit text size
SDL_Surface* TextListView::renderText(const std::string& str) {
	tr.setColor(colText);
	return tr.render(str);
}


void TextListView::scrollBarCallback(const int contentOffset) {
	assert(!items.empty());
	assert(contentOffset >= 0);
	(void)contentOffset;
	updateVisible();
	updateDrawRange();
}


void TextListView::setDownItem(const std::size_t i) {
	if (i != downItem) {
		downItem = i;
		if (i < items.size())
			callback(items[downItem]);
		else
			callback(nullptr);
	}
}


// y relative
std::size_t TextListView::getItemY(const int y) {
	if (y < 0)
		return Constants::maxIndex;
	const int yAbs = bar.getContentOffset() + y;
	const std::size_t i = static_cast<std::size_t>(yAbs / itemHeight);
	if (i >= items.size())
		return Constants::maxIndex;
	const TextItem* const item = items[i];
	if ((yAbs >= item->getPos().second) && (yAbs < (item->getPos().second + itemHeight)))
		return i;
	else
		return Constants::maxIndex;
}


//! TODO run in constant time
void TextListView::updateVisible() {
	const std::size_t lo = static_cast<std::size_t>(bar.getContentOffset() / itemHeight);
	const std::size_t hi = std::min(
		static_cast<std::size_t>((bar.getContentOffset() + bounds.h - 1) / itemHeight),
		items.size() - 1
	);
	if (visibleStart == Constants::maxIndex) {
		visibleStart = lo;
		visibleEnd = hi;
		for (std::size_t i = visibleStart; i <= visibleEnd; ++i)
			items[i]->setVisible(true);
		return;
	}
	const std::size_t end = std::max(visibleEnd, hi);
	for (std::size_t i = std::min(visibleStart, lo); i <= end; ++i) {
		if ((i >= lo) && (i <= hi))
			items[i]->setVisible(true);
		else
			items[i]->setVisible(false);
	}
	visibleStart = lo;
	visibleEnd = hi;
}


void TextListView::updateDrawRange() {
	drawItems.clear();
	if (items.empty())
		return;
	DrawRange dr;
	dr.lo = visibleStart;
	dr.hi = visibleEnd;
	dr.state = WidgetState::OUT;
	drawItems.push_back(dr);
	if ((overItem == downItem) && inRange(downItem, visibleStart, visibleEnd)) {
		insert(drawItems, downItem, WidgetState::DOWN);
	}
	else {
		if (inRange(overItem, visibleStart, visibleEnd))
			insert(drawItems, overItem, WidgetState::OVER);
		if (inRange(downItem, visibleStart, visibleEnd))
			insert(drawItems, downItem, WidgetState::DOWN);
	}
}


void TextListView::updateScrollBarPos() {
	bar._setPos(IntPair{bounds.w - bar.getSize().first, 0});
	bar._resize(IntPair{bar.getSize().first, bounds.h}, WidgetResizeFlag::PARENT);
}


void TextListView::reset() {
	drawItems.clear();
	// set visible to invisible
	if (visibleStart != Constants::maxIndex) {
		for (std::size_t i = visibleStart; i <= visibleEnd; ++i)
			items[i]->setVisible(false);
	}
	visibleStart = Constants::maxIndex;
	overItem = Constants::maxIndex;
	downItem = Constants::maxIndex;
}
