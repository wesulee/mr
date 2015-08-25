#pragma once


// Various constants used by changing state and processing events
namespace Parameters {
	enum : int {
		// mIntInt, key to MainGame if new game
		NEW_GAME,
		// mIntStr, key to MainGame if load game, value is save file
		LOAD_GAME,
		// mIntInt, key provided by MainGame, value non-zero if still running else zero
		MAIN_GAME_IS_RUNNING,
		// event, pushed by CreatureManager when empty
		CREATURES_EMPTY
	};
}
