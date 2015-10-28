#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>


namespace Constants {
	typedef float float_type;
	// 16:9 aspect ratio
	constexpr int windowWidth = 576;
	constexpr int windowHeight = 324;
	constexpr int roomWidth = 32 * 18;
	constexpr int roomHeight = 32 * 9;
	constexpr int maxFrameSkip = 3;
	constexpr float floatInc = 0.01;
	constexpr char loggerFName[] = "errors.txt";
	constexpr char iniFileName[] = "mr.ini";
	constexpr char saveFileExt[] = "txt";
	constexpr std::size_t maxIndex = std::numeric_limits<std::size_t>::max();
	// Console
	constexpr bool ConsoleMilliseconds = true;
	// JSONReader
	constexpr std::size_t JSONBufferSz = (8 * 1024);
	// Map
	constexpr int MapCountX = 12;
	constexpr int MapCountY = 6;
	constexpr int MapBlockSz = 8;
	// Player
	constexpr int PHealth = 100;
	constexpr float PBaseMovSpeed = 100.0f;		// base movement speed per axis (pps)
	// PlayerHealthBar
	constexpr float PHealthGradRatio = 3.0f;	// gradient accuracy, lower is better
	constexpr int PHealthBarWidth = 100;
	constexpr int PHealthBarHeight = 12;
	constexpr int PHealthPosX = 10;
	constexpr int PHealthPosY = roomHeight + (windowHeight - roomHeight) / 2 - 2;
	// ResourceManager
	constexpr int RMRoomLen = 2;
	// Room
	constexpr int RoomX = 0;	// offset
	constexpr int RoomY = 18;	// offset
	constexpr std::size_t RoomQTreeNodeCap = 8;
	// SpellManager
	constexpr float SMTravelSpeed = 500.0f;	// pixels per second spell travel
	constexpr float SMFadeDur = 0.130;		// fadeout duration
	constexpr float SMFadeRadMult = 1.6;	// fadeout radius multiplier
	constexpr int SPELL_RENDER_RADIUS = 32;
	// Widget
	// Special argument for Widget::_requestResize(), lets parent know that widget
	//   wants to use as much space as possible.
	constexpr int WSizeExpand = std::numeric_limits<int>::max();
	// Special argument for WidgetLayout::setSpacing(). Spacing between widgets will be
	//  dynamic to distribute widgets over both ends of layout.
	constexpr int WSpacingExpand = std::numeric_limits<int>::min();
	// maximum time (ms) between mouse press and release to be considered a click
	constexpr uint32_t WClickDur = 100;
	constexpr uint32_t WTextCursorBlinkRate = 500;	// duration (ms) of a tick
}


// If Windows, make sure _WIN32 is defined
#if defined(WIN32) || defined(__WIN32)
	#ifndef _WIN32
		#define _WIN32
	#endif
#endif



#ifndef NDEBUG
#include "console.h"
// Begin new line
#define DEBUG_BEGIN Console::begin()
// Debug ostream
#define DEBUG_OS Console::get()
// Alpha are percentages (0 transparent, 100 opaque)
// Mouse position overlay
#define DEBUG_MOUSE_POS       1
#define DEBUG_MOUSE_POS_SZ    5
#define DEBUG_MOUSE_POS_COLOR COLOR_CYAN
// SDL_Event
#define DEBUG_EVENT_PREPEND "SDLEvent "
#define DEBUG_EVENT_MOUSEBUTTONDOWN 0
#define DEBUG_EVENT_MOUSEBUTTONUP   0
#define DEBUG_EVENT_QUIT            1
#define DEBUG_EVENT_TEXTINPUT       1
#define DEBUG_EVENT_TEXTEDITING     1
#define DEBUG_EVENT_USEREVENT       1
#define DEBUG_EVENT_WINDOWEVENT     0
// Creatures
#define DEBUG_CREATURE_BOUNDS       1
#define DEBUG_CREATURE_BOUNDS_COLOR COLOR_CYAN
#define DEBUG_CREATURE_BOUNDS_ALPHA 33
// InputHandler
#define DEBUG_IH_PREPEND "InputHandler "
#define DEBUG_IH_TEXT_INPUT 1
// JSON
#define DEBUG_JSON_PREPEND "JSON "
#define DEBUG_JSON_READ     1
#define DEBUG_JSON_VALIDATE 1
// ResourceManager
#define DEBUG_RM_PREPEND "ResMan "
#define DEBUG_RM_IMG_PREPEND "imgRef "
#define DEBUG_RM_SS_PREPEND "sprSheet "
#define DEBUG_RM_LOAD_FONT        1
#define DEBUG_RM_UNLOAD_FONT      1
#define DEBUG_RM_LOAD_ANIMATION   1
#define DEBUG_RM_UNLOAD_ANIMATION 1
#define DEBUG_RM_IMG_REF          1
#define DEBUG_RM_SS_REF           1
// Room
#define DEBUG_ROOM_BLOCK       1
#define DEBUG_ROOM_BLOCK_COLOR COLOR_RED
#define DEBUG_ROOM_BLOCK_ALPHA 33
#define DEBUG_ROOM_BOUNDS       1
#define DEBUG_ROOM_BOUNDS_SIZE  3
#define DEBUG_ROOM_BOUNDS_COLOR COLOR_CYAN
#define DEBUG_ROOM_BOUNDS_ALPHA 50
// StateManager
#define DEBUG_StM_PREPEND "StateManager "
#define DEBUG_StM_GS      1
#define DEBUG_StM_NEW_DEL 1
#define DEBUG_StM_STACK   0
// Widget buttons
// (DEBUG_TEXTBUTTON_* also applies to TextButton2)
#define DEBUG_TEXTBUTTON_DRAWTEXTURE       1
#define DEBUG_TEXTBUTTON_DRAWTEXTURE_COLOR COLOR_RED
#define DEBUG_TEXTBUTTON_DRAWTEXTURE_ALPHA 33
// WidgetEvent
#define DEBUG_WE_PREPEND "WidgetEvent "
#define DEBUG_WE_MOUSE_CLICK   0
#define DEBUG_WE_MOUSE_MOVE    0
#define DEBUG_WE_MOUSE_PRESS   0
#define DEBUG_WE_MOUSE_RELEASE 0
#define DEBUG_WE_TEXT_DELETE   1
#define DEBUG_WE_TEXT_EDIT     1
#define DEBUG_WE_TEXT_INPUT    1
// WidgetLayout
#define DEBUG_WL_BOUNDS             0
#define DEBUG_WL_BOUNDS_BORDER_SZ   3
#define DEBUG_WL_BOUNDS_BORDERCOLOR COLOR_WHITE
#define DEBUG_WL_BOUNDS_FILLCOLOR   COLOR_RED
#define DEBUG_WL_BOUNDS_FILLALPHA   20
#define DEBUG_WL_CHILDBOUNDS             0
#define DEBUG_WL_CHILDBOUNDS_BORDER_SZ   1
#define DEBUG_WL_CHILDBOUNDS_BORDERCOLOR COLOR_WHITE
#define DEBUG_WL_CHILDBOUNDS_FILLCOLOR   COLOR_CYAN
#define DEBUG_WL_CHILDBOUNDS_FILLALPHA   40
#endif // NDEBUG
