#pragma once

#include "constants.h"
#include "exception.h"
#include "shapes.h"
#include "widget.h"
#include "widget_event.h"
#include "widget_group.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>


namespace LayoutHelper {

// first is width, second is height
typedef std::pair<bool, bool> ExpandPreference;


void drawWidgets(Canvas&, const SDL_Rect&, std::vector<Widget*>&);
void drawBounds(Canvas&, const SDL_Rect&);
void fillBounds(Canvas&, const SDL_Rect&);


// Find last widget in widgets such that PosFunc(w->getPos()) <= val
// Simply finds the widget at the given position if there is one, or the closest widget before position.
// For vert layout: PosFunc gets y coordinate of pos, val is y coordinate of pos to look for
template<int (*PosFunc)(const IntPair&)>
Widget* findWidget(const int val, const std::vector<Widget*>& widgets) {
	if (widgets.empty() || (val < PosFunc(widgets.front()->getPos())))
		return nullptr;
	// search [lo, hi]
	std::size_t lo = 0;
	std::size_t hi = widgets.size() - 1;
	std::size_t mid;
	int cval;
	while ((hi - lo) > 2) {
		mid = lo + (hi - lo) / 2;
		cval = PosFunc(widgets[mid]->getPos());
		if (cval > val)
			hi = mid - 1;	// mid is past target, so lower hi
		else
			lo = mid;		// mid satisfies condition, so low can be raised
	}
	// reverse iterate [hi, lo]
	for (std::size_t i = hi + 1; i-- > lo;) {
		if (PosFunc(widgets[i]->getPos()) <= val)
			return widgets[i];
	}
	return widgets[lo];
}


inline
uintptr_t toMapKey(const Widget* const w) {
	return reinterpret_cast<uintptr_t>(w);
}


inline
bool checkExists(const std::vector<Widget*>& vec, const Widget* const w) {
	return (std::find(vec.begin(), vec.end(), w) != vec.cend());
}


inline
bool checkPrefExpand(const IntPair& p) {
	return (
		(p.first == Constants::WSizeExpand)
		|| (p.second == Constants::WSizeExpand)
	);
}


// would use std::max_element, but want to minimize calls to size function
template<class Iterator, class Function>
static Iterator getMax(Iterator beg, const Iterator end, Function func) {
	if (beg == end)
		return end;
	Iterator maxIter = beg;
	auto maxResult = func(*maxIter);
	auto tmp = maxResult;
	for (++beg; beg != end; ++beg) {
		tmp = func(*beg);
		if (tmp > maxResult) {
			maxResult = tmp;
			maxIter = beg;
		}
	}
	return maxIter;
}

}	// namespace LayoutHelper


class WidgetLayout : public Widget {
	typedef WidgetLayout self_type;
public:
	// when provided to setSpacing()
	WidgetLayout() = default;
	virtual ~WidgetLayout() = default;
	virtual void add(Widget*) = 0;
	virtual void setMargins(const int, const int, const int, const int);
	virtual void setWidgetAlignment(const WidgetAlignmentHoriz, const WidgetAlignmentVert);
	virtual void setSpacing(const int);		// see Constants::WSpacingExpand
	// Widget methods
	void event(WidgetEvent&) override;
protected:
	bool updateMargins(const int, const int, const int, const int);
	bool updateWidgetAlignment(const WidgetAlignmentHoriz, const WidgetAlignmentVert);
	void updateContentBounds(void);

	LayoutWidgetGroup group;
	SDL_Rect contentBounds;
	int spacing = 5;	// spacing between widgets (children)
	int marginT = 5;	// top
	int marginB = 5;	// bottom
	int marginL = 5;	// left
	int marginR = 5;	// right
	WidgetAlignmentHoriz alignH = WidgetAlignmentHoriz::CENTER;
	WidgetAlignmentVert alignV = WidgetAlignmentVert::TOP;
};


/*
Base class of VerticalLayout and HorizontalLayout
Methods that contain P stand for primary (prim), S for secondary (sec)
The primary dimension of vertical layout is height, horizontal layout is width
*/
template<class T>
class AbstractLayout : public WidgetLayout {
	AbstractLayout(const AbstractLayout&) = delete;
	void operator=(const AbstractLayout&) = delete;
	typedef AbstractLayout self_type;
public:
	AbstractLayout();
	~AbstractLayout();
	// Layout implementation
	void add(Widget*) override;
	void setMargins(const int, const int, const int, const int) override;
	void setWidgetAlignment(const WidgetAlignmentHoriz, const WidgetAlignmentVert) override;
	void setSpacing(const int) override;
	// Widget implementation
	void draw(Canvas&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _requestResize(Widget*, const IntPair&) override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	void addExpand(Widget*);
	void updateLayout(void);
	void updateLayoutExpand(void);
	void childResize(Widget*, const IntPair&);
	void insertWidgetsExpand(const Widget*);
	void removeWidgetsExpand(const Widget*);
	void addHelper(Widget*);
	Widget* widgetLookup(const int, const int) const;
	void updatePosSFunc(void);
	int getPosSVAlignLeft(const Widget*) const;
	int getPosSVAlignCenter(const Widget*) const;
	int getPosSVAlignRight(const Widget*) const;

	// Widgets which prefer to have expanded dimensions (w, h, or both) (this excludes widgets
	//   with sizePolicy of FIXED). Rather than use Widget* as key, use integer type to avoid
	//   the const headaches.
	std::unordered_map<uintptr_t, LayoutHelper::ExpandPreference> widgetsExpand;
	std::vector<Widget*> widgets;	// all widgets (including those in widgetsExpand)
	int (self_type::*funcPosS)(const Widget*) const;	// get position of sec. coord of a widget
	// This is the min size that layout should be resized to in order to fut all content
	// For vert layout: width of minWidget, second is sum of heights of getMinSize of all widgets
	// For horiz layout: sum of widths of getMinSize, second is height of minWidget
	IntPair minSize;
	// The widget with the largest secondary size
	// For vert layout: the widget with the largest width of getSize()
	// For horiz layout: the widget with the largest height of getSize()
	Widget* maxWidget = nullptr;
	// The widget with the largest secondary min size
	// For vert layout: the widget with the largest width of getMinSize()
	// For horiz layout: the widget with the largest height of getMinSize()
	Widget* minWidget = nullptr;
	// The sec size of maxWidget
	// For vert layout: the width of maxWidget
	// For horiz layout: the height of maxWidget
	int maxSize = 0;
	// The sum of all primary current sizes
	// For vert layout: sum of heights of all getSize()
	// For horiz layout: sum of widths of all getSize()
	int contentSize = 0;
	// contentSize excluding widgets where secondary getPrefSize() is expand
	// (Note: this is a summation of getSize(), not getPrefSize())
	// For vert layout: sum of heights excluding where getPrefSize() height is expand
	// For horiz layout: sum of widths excluding where getPrefSize() width is expand
	int contentSizeExc = 0;
	// # of widgets that prefer expanded primary size
	// For vert layout: # of widgets that prefer height expanded
	// For horiz layout: # of widgets that prefer width expanded
	int prefExpandPCount = 0;		// # of children that prefer height expanded
	// # of widgets that prefer expanded secondary size
	// For vert layout: # of widgets that prefer width expanded
	// For horiz layout: # of widgets that prefer height expanded
	int prefExpandSCount = 0;		// # of children that prefer width expanded
};


struct VerticalLayoutBase {
	static int& getPRef(IntPair& p) {
		return p.second;
	}

	static int& getSRef(IntPair& p) {
		return p.first;
	}

	static int getP(const IntPair& p) {
		return p.second;
	}

	static int getS(const IntPair& p) {
		return p.first;
	}

	static int getSzP(const SDL_Rect& r) {
		return r.h;
	}

	static int getSzS(const SDL_Rect& r) {
		return r.w;
	}

	static bool& getPRef(LayoutHelper::ExpandPreference& ep) {
		return ep.second;
	}

	static bool& getSRef(LayoutHelper::ExpandPreference& ep) {
		return ep.first;
	}

	static Widget* getWidget(const IntPair& pos, const std::vector<Widget*>& widgets) {
		return LayoutHelper::findWidget<getP>(getP(pos), widgets);
	}

	static bool vert() {
		return true;
	}
};


struct HorizontalLayoutBase {
	static int& getPRef(IntPair& p) {
		return p.first;
	}

	static int& getSRef(IntPair& p) {
		return p.second;
	}

	static int getP(const IntPair& p) {
		return p.first;
	}

	static int getS(const IntPair& p) {
		return p.second;
	}

	static int getSzP(const SDL_Rect& r) {
		return r.w;
	}

	static int getSzS(const SDL_Rect& r) {
		return r.h;
	}

	static bool& getPRef(LayoutHelper::ExpandPreference& ep) {
		return ep.first;
	}

	static bool& getSRef(LayoutHelper::ExpandPreference& ep) {
		return ep.second;
	}

	static Widget* getWidget(const IntPair& pos, const std::vector<Widget*>& widgets) {
		return LayoutHelper::findWidget<getP>(getP(pos), widgets);
	}

	static bool vert() {
		return false;
	}
};


class VerticalLayout : public AbstractLayout<VerticalLayoutBase> {
};


class HorizontalLayout : public AbstractLayout<HorizontalLayoutBase> {
};


// AbstractLayout implementation


template<class T>
AbstractLayout<T>::AbstractLayout() : minSize(0, 0) {
	using namespace std::placeholders;
	updateContentBounds();
	group.setBounds(&bounds);
	group.setLookupFunc(std::bind(&self_type::widgetLookup, this, _1, _2));
	updatePosSFunc();
}


template<class T>
AbstractLayout<T>::~AbstractLayout() {
	for (auto w : widgets)
		delete w;
}


/*
Calculate size that layout should resize to to fit widget, resize widget to minimum
  size, resize layout, update layout (updating size of new widget as well).
Special case: layout sizePolicy is FIXED.
Special case: widget sizePolicy is FIXED. Do not resize widget.
Special case: getPrefSize() is expand. Resize widget to min size. If no parent,
  need to pick some size to resize to.
*/
template<class T>
void AbstractLayout<T>::add(Widget* w) {
	using namespace LayoutHelper;
	if (w == nullptr)
		logAndExit(RuntimeError{"AbstractLayout::add nullptr"});
	w->_setParent(this);
	w->_setIndex(widgets.size());
	widgets.push_back(w);
	IntPair prefSz = w->getPrefSize();
	if ((w->getSizePolicy() != WidgetSizePolicy::FIXED) && checkPrefExpand(prefSz)) {
		addExpand(w);
		return;
	}
	IntPair curSz = w->getSize();
	IntPair minSz = w->getMinSize();
	IntPair resSz;	// min size that layout needs to resize to to fit widget
	if ((curSz != minSz) && (w->getSizePolicy() != WidgetSizePolicy::FIXED)) {
		w->_resize(minSz, WidgetResizeFlag::PARENT);
		assert(w->getSize() == minSz);
		curSz = w->getSize();
		prefSz = w->getPrefSize();
		minSz = w->getMinSize();
		assert(!checkPrefExpand(prefSz));
	}
	// update contentSize, contentSizeExc
	contentSize += T::getP(curSz);
	contentSizeExc += T::getP(curSz);
	addHelper(w);
	// calc resize size
	// initially assume no expand widgets
	resSz.first = marginT + marginB;
	resSz.second = marginL + marginR;
	T::getPRef(resSz) += contentSizeExc;
	if (w->getSizePolicy() == WidgetSizePolicy::FIXED) {
		if (T::getS(curSz) > maxSize)
			T::getSRef(resSz) += T::getS(curSz);
		else
			T::getSRef(resSz) += maxSize;
		T::getPRef(resSz) += T::getP(curSz);
	}
	else {
		if (T::getS(prefSz) > maxSize)
			T::getSRef(resSz) += T::getS(prefSz);
		else
			T::getSRef(resSz) += maxSize;
		T::getPRef(resSz) += T::getP(prefSz);
	}
	// add spacing
	if (spacing == Constants::WSpacingExpand)
		T::getPRef(resSz) += static_cast<int>(widgets.size() * 10);
	else
		T::getPRef(resSz) += static_cast<int>(widgets.size() - 1) * spacing;
	// do resize
	if (_getParent() == nullptr) {
		if (prefExpandPCount > 0) {
			// Add primary size contribution from primary expand widgets
			// Since no parent, all primary expand widgets' primary size is their min size,
			//   then (contentSize - contentSizeExc) should be combined min size of those widgets
			T::getPRef(resSz) += (contentSize - contentSizeExc);
		}
		if (sizePolicy != WidgetSizePolicy::FIXED)
			_resize(resSz, WidgetResizeFlag::SELF);
	}
	else {
		if (prefExpandPCount > 0)
			T::getPRef(resSz) = Constants::WSizeExpand;
		if (prefExpandSCount > 0)
			T::getSRef(resSz) = Constants::WSizeExpand;
		if (sizePolicy != WidgetSizePolicy::FIXED)
			_getParent()->_requestResize(this, resSz);
	}
	updateLayout();
}


template<class T>
void AbstractLayout<T>::setMargins(const int t, const int b, const int l, const int r) {
	if (updateMargins(t, b, l, r)) {
		updateContentBounds();
		updateLayout();
	}
}


template<class T>
void AbstractLayout<T>::setWidgetAlignment(const WidgetAlignmentHoriz h, const WidgetAlignmentVert v) {
	if (updateWidgetAlignment(h, v)) {
		updatePosSFunc();
		updateLayout();
	}
}


template<class T>
void AbstractLayout<T>::setSpacing(const int n) {
	assert(n >= 0);
	if (n == spacing)
		return;
	spacing = n;
	updateLayout();
}


template<class T>
void AbstractLayout<T>::draw(Canvas& can) {
	#if defined(DEBUG_WL_BOUNDS) && DEBUG_WL_BOUNDS
		LayoutHelper::fillBounds(can, bounds);
		LayoutHelper::drawWidgets(can, contentBounds, widgets);
		LayoutHelper::drawBounds(can, bounds);
	#else
		LayoutHelper::drawWidgets(can, contentBounds, widgets);
	#endif
}


template<class T>
IntPair AbstractLayout<T>::getPrefSize() const {
	IntPair ret;
	ret.first = marginL + marginR;
	ret.second = marginT + marginB;
	// set secondary
	if (prefExpandSCount > 0)
		T::getSRef(ret) = Constants::WSizeExpand;
	else
		T::getSRef(ret) += maxSize;
	// set primary
	if (prefExpandPCount > 0)
		T::getPRef(ret) = Constants::WSizeExpand;
	else if (spacing == Constants::WSpacingExpand)
		T::getPRef(ret) += (static_cast<int>(widgets.size() * 10) + contentSize);
	else {
		T::getPRef(ret) += contentSize;
		if (!widgets.empty())
			T::getPRef(ret) += spacing * static_cast<int>(widgets.size() - 1);
	}
	return ret;
}


template<class T>
IntPair AbstractLayout<T>::getMinSize() const {
	if (widgets.empty())
		return IntPair{1, 1};
	else
		return minSize;
}


template<class T>
void AbstractLayout<T>::_requestResize(Widget* w, const IntPair&) {
	assert(LayoutHelper::checkExists(widgets, w));
	//! TODO not implemented
	(void)w;
	assert(false);
}


template<class T>
void AbstractLayout<T>::_resize(const IntPair& p, const WidgetResizeFlag f) {
	assert((p.first != Constants::WSizeExpand) && (p.second != Constants::WSizeExpand));
	bounds.w = p.first;
	bounds.h = p.second;
	updateContentBounds();
	switch (f) {
	case WidgetResizeFlag::NONE:
	case WidgetResizeFlag::PARENT:
		updateLayout();
		break;
	default:
		break;
	}
}


/*
Determining resize size:
  -When no parent, _resize() is to be called directly, but WSizeExpand is not valid, so some value
   must be chosen.
The resize size (per dimension) can be arbitrary when:
  -no parent and prefExpand_Count is non-zero
If no widgets want primary expanded, then the resize primary should be
  contentSize excluding new widget plus the preferred height of new widget
*/
template<class T>
void AbstractLayout<T>::addExpand(Widget* w) {
	using namespace LayoutHelper;
	assert(checkPrefExpand(w->getPrefSize()));
	assert(w->getSizePolicy() != WidgetSizePolicy::FIXED);	//! TODO case not handled
	insertWidgetsExpand(w);
	IntPair minSz = w->getMinSize();
	// do not call childResize() to resize
	w->_resize(minSz, WidgetResizeFlag::PARENT);
	assert(w->getSize() == minSz);	// confirm widget resized bounds correctly
	IntPair curSz = w->getSize();
	IntPair prefSz = w->getPrefSize();
	IntPair resSz;	// resize size
	if (!checkPrefExpand(prefSz))
		logAndExit(RuntimeError{"VerticalLayout::addExpand no expand after resize"});
	// update contentSize, contentSizeExc
	contentSize += T::getP(curSz);
	if (T::getP(prefSz) != Constants::WSizeExpand)
		contentSizeExc += T::getP(curSz);
	addHelper(w);
	// calc resize size
	resSz.first = marginL + marginR;
	resSz.second = marginT + marginB;
	T::getPRef(resSz) += contentSize;
	T::getSRef(resSz) += maxSize;
	// if primary pref size is not expand, then resize size should be large enough
	// to resize widget to its primary pref size
	if (T::getP(prefSz) != Constants::WSizeExpand) {
		assert(T::getP(prefSz) != Constants::WSizeExpand);
		T::getPRef(resSz) += T::getP(prefSz) - T::getP(curSz);
	}
	if (_getParent() != nullptr) {
		if (prefExpandPCount > 0)
			T::getPRef(resSz) = Constants::WSizeExpand;
		if (prefExpandSCount > 0)
			T::getSRef(resSz) = Constants::WSizeExpand;
	}
	// add spacing
	if (T::getP(resSz) != Constants::WSizeExpand) {
		if (spacing == Constants::WSpacingExpand)
			T::getPRef(resSz) += static_cast<int>(widgets.size() * 10);
		else
			T::getPRef(resSz) += static_cast<int>(widgets.size() - 1) * spacing;
	}
	if (sizePolicy != WidgetSizePolicy::FIXED)
		_doResize(resSz);
	updateLayout();
}


template<class T>
void AbstractLayout<T>::updateLayout() {
	// Note: all positions are relative to contentBounds
	if (widgets.empty())
		return;
	if (spacing == Constants::WSpacingExpand) {
		updateLayoutExpand();
		return;
	}
	IntPair curSz;		// current size
	IntPair prefSz;		// preferred size
	IntPair resSz;		// resize size
	IntPair position;
	int spacingSz = static_cast<int>(widgets.size() - 1) * spacing;	// total spacing size
	int expandSz = -1;	// primary size of all widgets with expanded primary (default value)
	if (prefExpandPCount > 0) {
		// equally share available space
		expandSz = (T::getSzP(contentBounds) - contentSizeExc - spacingSz) / prefExpandPCount;
		assert(expandSz > 0);
	}
	// determine starting primary position of widget
	if (T::vert()) {	// vertical layout
		switch (alignV) {
		case WidgetAlignmentVert::TOP:
			T::getPRef(position) = 0;
			break;
		case WidgetAlignmentVert::CENTER:
			if (prefExpandPCount > 0)
				T::getPRef(position) = 0;
			else
				T::getPRef(position) = (T::getSzP(contentBounds) - contentSize - spacingSz) / 2;
			break;
		case WidgetAlignmentVert::BOTTOM:
			if (prefExpandPCount > 0)
				T::getPRef(position) = 0;
			else
				T::getPRef(position) = T::getSzP(contentBounds) - contentSize - spacingSz;
			break;
		}
	}
	else {		// horizontal layout
		switch (alignH) {
		case WidgetAlignmentHoriz::LEFT:
			T::getPRef(position) = 0;
			break;
		case WidgetAlignmentHoriz::CENTER:
			if (prefExpandPCount > 0)
				T::getPRef(position) = 0;
			else
				T::getPRef(position) = (T::getSzP(contentBounds) - contentSize - spacingSz) / 2;
			break;
		case WidgetAlignmentHoriz::RIGHT:
			if (prefExpandPCount > 0)
				T::getPRef(position) = 0;
			else
				T::getPRef(position) = T::getSzP(contentBounds) - contentSize - spacingSz;
			break;
		}
	}
	for (auto w : widgets) {
		curSz = w->getSize();
		prefSz = w->getPrefSize();
		resSz = prefSz;
		if (T::getS(prefSz) == Constants::WSizeExpand)
			T::getSRef(resSz) = T::getSzS(contentBounds);
		else if (T::getS(prefSz) != T::getS(curSz))
			T::getSRef(resSz) = T::getS(prefSz);
		if (T::getP(prefSz) == Constants::WSizeExpand)
			T::getPRef(resSz) = expandSz;
		else if (T::getP(prefSz) != T::getP(curSz))
			T::getPRef(resSz) = T::getP(prefSz);
		if ((curSz != resSz) && (w->getSizePolicy() != WidgetSizePolicy::FIXED))
			childResize(w, resSz);
		T::getSRef(position) = (this->*funcPosS)(w);
		w->_setPos(position);
		T::getPRef(position) += spacing + T::getP(w->getSize());
	}
}


template<class T>
void AbstractLayout<T>::updateLayoutExpand() {
	assert(false);	// not implemented
}


// Resizes the widget to given size
// After resize, the widget's min and pref size may change, including changing pref expand
// Updates maxWidget, maxSize, minWidget, minSize, contentSize, contentSizeExc,
//   widgetsExpand, prefExpandPCount, prefExpandSCount
template<class T>
void AbstractLayout<T>::childResize(Widget* w, const IntPair& p) {
	using namespace LayoutHelper;
	assert(!widgets.empty());
	assert(w != nullptr);
	IntPair prevSz = w->getSize();
	IntPair prevPrefSz = w->getPrefSize();
	IntPair prevMinSz = w->getMinSize();
	IntPair curSz;
	IntPair curPrefSz;
	IntPair curMinSz;
	w->_resize(p, WidgetResizeFlag::PARENT);
	assert(w->getSize() == p);	// make sure resized to correct size
	curSz = w->getSize();
	if (curSz != p)
		logAndExit(RuntimeError{"AbstractLayout::childResize"});
	curPrefSz = w->getPrefSize();
	curMinSz = w->getMinSize();
	// insert or remove from widgetsExpand if necessary
	//! TODO current does not update widgetsExpand values when existing widget changes pref expand
	const bool prevExpand = checkPrefExpand(prevPrefSz);
	const bool curExpand = checkPrefExpand(curPrefSz);
	if (prevExpand != curExpand) {
		if (prevExpand) {
			// previously preferred expand, now no longer prefer expand
			assert(!curExpand);
			removeWidgetsExpand(w);
		}
		else {
			// previously did not prefer expand, now it does
			assert(curExpand);
			insertWidgetsExpand(w);
		}
	}
	// update contentSize, contentSizeExc
	contentSize += (T::getP(curSz) - T::getP(prevSz));
	if (T::getP(prevPrefSz) == Constants::WSizeExpand) {
		if (T::getP(curPrefSz) == Constants::WSizeExpand)
			;	// no change
		else {
			// previously prefer expand, now does not
			contentSizeExc += T::getP(curSz);
		}
	}
	else {
		if (T::getP(curPrefSz) == Constants::WSizeExpand) {
			// previous did not prefer expand, now it does
			contentSizeExc -= T::getP(prevSz);
		}
		else {
			// previously and currently does not prefer expand, so update change in size
			contentSizeExc += (T::getP(curSz) - T::getP(prevSz));
		}
	}
	// update maxWidget, maxSize
	if (w == maxWidget) {
		if ((T::getS(curSz) - maxSize) < 0) {
			maxWidget = *getMax(
				widgets.cbegin(),
				widgets.cend(),
				[](const Widget* const a) {return T::getS(a->getSize());}
			);
			maxSize = T::getS(maxWidget->getSize());
		}
		else {
			maxSize = T::getS(curSz);
		}
	}
	else if (T::getS(curSz) > maxSize) {
		maxWidget = w;
		maxSize = T::getS(curSz);
	}
	// update minWidget, getSRef(minSize)
	if (w == minWidget) {
		if ((T::getS(curMinSz) - T::getS(prevMinSz)) < 0) {
			// sec. min size has decreased (width if vert layout), so find new minWidget
			minWidget = *getMax(
				widgets.cbegin(),
				widgets.cend(),
				[](const Widget* const a) {return T::getS(a->getMinSize());}
			);
			T::getSRef(minSize) = T::getS(minWidget->getMinSize());
		}
		else {
			// since sec. min size has not decreased, add change
			T::getSRef(minSize) += (T::getS(curMinSz) - T::getS(prevMinSz));
		}
	}
	else {
		T::getPRef(minSize) += (T::getP(curMinSz) - T::getP(prevMinSz));
		if (T::getS(curMinSz) > T::getS(minSize)) {
			minWidget = w;
			T::getSRef(minSize) = T::getS(curMinSz);
		}
	}
}


template<class T>
void AbstractLayout<T>::insertWidgetsExpand(const Widget* w) {
	using namespace LayoutHelper;
	assert(widgetsExpand.count(toMapKey(w)) == 0);
	IntPair prefSz = w->getPrefSize();
	ExpandPreference ep;
	ep.first = (prefSz.first == Constants::WSizeExpand);
	ep.second = (prefSz.second == Constants::WSizeExpand);
	assert(ep.first || ep.second);
	auto insert = widgetsExpand.emplace(toMapKey(w), ep);
	(void)insert;
	assert(insert.second);
	prefExpandPCount += static_cast<int>(T::getPRef(ep));
	prefExpandSCount += static_cast<int>(T::getSRef(ep));
}


template<class T>
void AbstractLayout<T>::removeWidgetsExpand(const Widget* w) {
	using namespace LayoutHelper;
	assert(widgetsExpand.count(toMapKey(w)) == 1);
	auto it = widgetsExpand.find(toMapKey(w));
	if (it == widgetsExpand.end())
		logAndExit(RuntimeError{"layout error"});
	prefExpandPCount -= static_cast<int>(T::getPRef(it->second));
	prefExpandSCount -= static_cast<int>(T::getSRef(it->second));
	widgetsExpand.erase(it);
}


// Note: widget does not get modified, but must pass as non-const
template<class T>
void AbstractLayout<T>::addHelper(Widget* w) {
	IntPair curSz = w->getSize();
	IntPair minSz = w->getMinSize();
	// update maxWidget
	if (T::getS(curSz) > maxSize) {
		maxWidget = w;
		maxSize = T::getS(curSz);
	}
	// update minWidget, minSize
	if (T::getS(minSz) > T::getS(minSize)) {
		minWidget = w;
		T::getSRef(minSize) = T::getS(minSz);
	}
	T::getPRef(minSize) += T::getP(minSz);
}


// x, y are relative to bounds
template<class T>
Widget* AbstractLayout<T>::widgetLookup(const int x, const int y) const {
	if (!containsWidgetRect(bounds, x, y))
		return nullptr;
	Widget* const w = T::getWidget(IntPair{x, y}, widgets);
	if ((w == nullptr) || !Shape::contains(w->getBounds(), x, y))
		return nullptr;
	else
		return w;
}


template<class T>
void AbstractLayout<T>::updatePosSFunc() {
	if (T::vert()) {
		switch (alignH) {
		case WidgetAlignmentHoriz::LEFT:
			funcPosS = &self_type::getPosSVAlignLeft;
			break;
		case WidgetAlignmentHoriz::CENTER:
			funcPosS = &self_type::getPosSVAlignCenter;
			break;
		case WidgetAlignmentHoriz::RIGHT:
			funcPosS = &self_type::getPosSVAlignRight;
			break;
		}
	}
	else {
		// no need to write new methods for horizontal layout since the same
		// methods as vertical layout as sufficient
		switch (alignV) {
		case WidgetAlignmentVert::TOP:
			funcPosS = &self_type::getPosSVAlignLeft;
			break;
		case WidgetAlignmentVert::CENTER:
			funcPosS = &self_type::getPosSVAlignCenter;
			break;
		case WidgetAlignmentVert::BOTTOM:
			funcPosS = &self_type::getPosSVAlignRight;
			break;
		}
	}
}


template<class T>
int AbstractLayout<T>::getPosSVAlignLeft(const Widget* w) const {
	(void)w;
	return 0;
}


template<class T>
int AbstractLayout<T>::getPosSVAlignCenter(const Widget* w) const {
	return ((T::getSzS(contentBounds) - T::getS(w->getSize())) / 2);
}


template<class T>
int AbstractLayout<T>::getPosSVAlignRight(const Widget* w) const {
	return (T::getSzS(contentBounds) - T::getS(w->getSize()));
}
