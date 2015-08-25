#include "widget_layout.h"
#include "canvas.h"


namespace LayoutHelper {

void drawWidgets(Canvas& can, const SDL_Rect& viewport, std::vector<Widget*>& widgets) {
	can.setRelViewport(viewport);
	for (auto w : widgets) {
		can.setRelViewport(w->getBounds());
		w->draw(can);
		can.clearViewport();
		#if defined(DEBUG_WL_CHILDBOUNDS) && DEBUG_WL_CHILDBOUNDS
			auto oldColor = can.getColorState();
			can.setColor(DEBUG_WL_CHILDBOUNDS_FILLCOLOR, getAlpha<DEBUG_WL_CHILDBOUNDS_FILLALPHA>());
			can.fillRect(w->getBounds());
			if (DEBUG_WL_CHILDBOUNDS_BORDER_SZ > 0) {
				can.setColor(DEBUG_WL_CHILDBOUNDS_BORDERCOLOR, SDL_ALPHA_OPAQUE);
				can.draw(w->getBounds(), DEBUG_WL_CHILDBOUNDS_BORDER_SZ);
			}
			can.setColorState(oldColor);
		#endif
	}
	can.clearViewport();
}


#ifndef NDEBUG
void fillBounds(Canvas& can, const SDL_Rect& bounds) {
	auto oldColor = can.getColorState();
	can.setColor(DEBUG_WL_BOUNDS_FILLCOLOR, getAlpha<DEBUG_WL_BOUNDS_FILLALPHA>());
	can.fillRect(0, 0, bounds.w, bounds.h);
	can.setColorState(oldColor);
}
#endif


#ifndef NDEBUG
void drawBounds(Canvas& can, const SDL_Rect& bounds) {
	// since viewport already inside bounds, need to draw relative to bounds
	if (DEBUG_WL_BOUNDS_BORDER_SZ > 0) {
		SDL_Rect relBounds;
		relBounds.x = 0;
		relBounds.y = 0;
		relBounds.w = bounds.w;
		relBounds.h = bounds.h;
		auto oldColor = can.getColorState();
		can.setColor(DEBUG_WL_BOUNDS_BORDERCOLOR, SDL_ALPHA_OPAQUE);
		can.draw(relBounds, DEBUG_WL_BOUNDS_BORDER_SZ);
		can.setColorState(oldColor);
	}
}
#endif

}	// namespace LayoutHelper


void WidgetLayout::setMargins(const int t, const int b, const int l, const int r) {
	assert((t >= 0) && (b >= 0) && (l >= 0) && (r >= 0));
	marginT = t;
	marginB = b;
	marginL = l;
	marginR = r;
	contentBounds.x = marginL;
	contentBounds.y = marginT;
	// follows from (cBounds.x + cBounds.w + marginR) == (bounds.x + bounds.w)
	contentBounds.w = bounds.x + bounds.w - contentBounds.x - marginR;
	contentBounds.h = bounds.y + bounds.h - contentBounds.y - marginB;
}


void WidgetLayout::setWidgetAlignment(const WidgetAlignmentHoriz h, const WidgetAlignmentVert v) {
	alignH = h;
	alignV = v;
}


void WidgetLayout::setSpacing(const int n) {
	assert(n >= 0);
	spacing = n;
}


void WidgetLayout::event(WidgetEvent& e) {
	group.event(e);
}


// if change has been made, update and return true, else return false
bool WidgetLayout::updateMargins(const int t, const int b, const int l, const int r) {
	if ((t != marginT) || (b != marginB) || (l != marginL) || (r != marginR)) {
		self_type::setMargins(t, b, l, r);
		return true;
	}
	return false;
}


bool WidgetLayout::updateWidgetAlignment(const WidgetAlignmentHoriz h, const WidgetAlignmentVert v) {
	if ((h != alignH) || (v != alignV)) {
		self_type::setWidgetAlignment(h, v);
		return true;
	}
	return false;
}


void WidgetLayout::updateContentBounds() {
	const int marginH = marginL + marginR;
	const int marginV = marginT + marginB;
	if (marginH >= bounds.w) {
		contentBounds.x = 0;	// remove horiz margin
		contentBounds.w = 0;
	}
	else {
		contentBounds.w = bounds.w - marginH;
	}
	if (marginV >= bounds.h) {
		contentBounds.y = 0;	// remove vert margin
		contentBounds.h = 0;
	}
	else {
		contentBounds.h = bounds.h - marginV;
	}
}
