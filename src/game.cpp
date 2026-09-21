#include "game.hpp"

#include <algorithm>
#include <random>

namespace {
Pos moved(Pos p, Direction d) {
  switch (d) {
    case Direction::Up: return {p.x, p.y - 1};
    case Direction::Down: return {p.x, p.y + 1};
    case Direction::Left: return {p.x - 1, p.y};
    case Direction::Right: return {p.x + 1, p.y};
  }
  return p;
}
}  // namespace

Game::Game(int width, int height) : w_(width), h_(height) { reset(); }

void Game::reset() {
  snake_.clear();
  const int cx = w_ / 2;
  const int cy = h_ / 2;
  // Head first, length 3, moving right.
  snake_.push_back({cx, cy});
  snake_.push_back({cx - 1, cy});
  snake_.push_back({cx - 2, cy});
  dir_ = Direction::Right;
  pending_ = Direction::Right;
  score_ = 0;
  gameOver_ = false;
  placeFood();
}

bool Game::isOpposite(Direction a, Direction b) {
  return (a == Direction::Up && b == Direction::Down) ||
         (a == Direction::Down && b == Direction::Up) ||
         (a == Direction::Left && b == Direction::Right) ||
         (a == Direction::Right && b == Direction::Left);
}

void Game::setDirection(Direction d) {
  // Ignore 180-degree reversals relative to committed direction.
  if (!isOpposite(d, dir_)) pending_ = d;
}

void Game::placeFood() {
  // Collect free cells; fine for typical CLI board sizes.
  std::deque<Pos> free;
  std::vector<std::vector<bool>> occ(h_, std::vector<bool>(w_, false));
  for (const auto& s : snake_) {
    if (s.x >= 0 && s.x < w_ && s.y >= 0 && s.y < h_) occ[s.y][s.x] = true;
  }
  for (int y = 0; y < h_; ++y)
    for (int x = 0; x < w_; ++x)
      if (!occ[y][x]) free.push_back({x, y});
  if (free.empty()) {
    gameOver_ = true;  // Board full: win counts as game over.
    return;
  }
  static thread_local std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<size_t> dist(0, free.size() - 1);
  food_ = free[dist(rng)];
}

bool Game::step() {
  if (gameOver_) return false;
  dir_ = pending_;
  const Pos next = moved(snake_.front(), dir_);

  // Wall collision.
  if (next.x < 0 || next.x >= w_ || next.y < 0 || next.y >= h_) {
    gameOver_ = true;
    return false;
  }
  const bool eats = (next == food_);
  // Self collision: tail cell frees up unless growing.
  const size_t checkLen = eats ? snake_.size() : snake_.size() - 1;
  for (size_t i = 0; i < checkLen; ++i) {
    if (snake_[i] == next) {
      gameOver_ = true;
      return false;
    }
  }

  snake_.push_front(next);
  if (eats) {
    ++score_;
    placeFood();
  } else {
    snake_.pop_back();
  }
  return !gameOver_;
}
