#include "bot.hpp"

#include <cstdlib>

namespace {
Pos moved(Pos p, Direction d) {
  switch (d) {
    case Direction::Up:
      return {p.x, p.y - 1};
    case Direction::Down:
      return {p.x, p.y + 1};
    case Direction::Left:
      return {p.x - 1, p.y};
    case Direction::Right:
      return {p.x + 1, p.y};
  }
  return p;
}

bool isOpposite(Direction a, Direction b) {
  return (a == Direction::Up && b == Direction::Down) ||
         (a == Direction::Down && b == Direction::Up) ||
         (a == Direction::Left && b == Direction::Right) ||
         (a == Direction::Right && b == Direction::Left);
}

int manhattan(Pos a, Pos b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}
}  // namespace

Direction pickMove(const Game& g) {
  const Direction cur = g.direction();
  // Current direction first for stable, non-jittery movement on ties.
  const Direction order[] = {cur, Direction::Up, Direction::Down, Direction::Left,
                             Direction::Right};
  const auto& snake = g.snake();
  const Pos food = g.food();

  Direction best = cur;
  int bestDist = -1;
  for (Direction d : order) {
    if (d != cur && isOpposite(d, cur)) {
      continue;
    }
    const Pos next = moved(g.head(), d);
    if (next.x < 0 || next.x >= g.width() || next.y < 0 || next.y >= g.height()) {
      continue;
    }
    // Same collision rule as Game::step: the tail cell frees up unless
    // this move eats.
    const bool eats = (next == food);
    const size_t checkLen = eats ? snake.size() : snake.size() - 1;
    bool hitsBody = false;
    for (size_t i = 0; i < checkLen; ++i) {
      if (snake[i] == next) {
        hitsBody = true;
        break;
      }
    }
    if (hitsBody) {
      continue;
    }
    const int dist = manhattan(next, food);
    if (bestDist < 0 || dist < bestDist) {
      bestDist = dist;
      best = d;
    }
  }
  return best;
}
