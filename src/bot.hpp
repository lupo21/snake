#pragma once

#include "game.hpp"

// Greedy autopilot: pure logic, no I/O. Returns the direction minimizing the
// Manhattan distance from the next head cell to the food, considering only
// moves that survive this tick (no 180 reversals, walls, or body hits under
// the same tail-frees-up rule as Game::step). Falls back to the current
// direction when no move survives.
Direction pickMove(const Game& g);
