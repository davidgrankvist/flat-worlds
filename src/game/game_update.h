#ifndef game_update_h
#define game_update_h

#include "game_state.h"

LIBGAME_EXPORT void GameUpdate(float deltaTime, GameState* gameState);
LIBGAME_EXPORT void InitGameState(GameState* gameState);

#endif

