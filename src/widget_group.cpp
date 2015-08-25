#include "widget_group.h"
#include "widget.h"
#include "widget_event.h"
#include <cassert>


static void sendEvent(Widget* const w, WidgetEvent& e) {
	if (w != nullptr)
		w->event(e);
}


void LayoutWidgetGroup::event(WidgetEvent& e) {
	WEMouseAutoUpdate mTest;
	mTest.set(e, *bounds);
	switch (e.type) {
	case WidgetEventType::MOUSE_MOVE:
		if (selectedExclusive) {
			sendEvent(selected, e);
		}
		else {
			Widget* w = lookup(e.move.x, e.move.y);
			if (w == selected) {
				sendEvent(selected, e);
			}
			else {
				sendEvent(selected, e);
				selected = w;
				sendEvent(selected, e);
			}
		}
		break;
	case WidgetEventType::MOUSE_PRESS:
		selected = lookup(e.press.x, e.press.y);
		selectedExclusive = true;
		sendEvent(selected, e);
		break;
	case WidgetEventType::MOUSE_RELEASE:
		selectedExclusive = false;
		sendEvent(selected, e);
		break;
	case WidgetEventType::MOUSE_CLICK:
		sendEvent(selected, e);
		break;
	default:
		assert(false);
		break;
	}
}
