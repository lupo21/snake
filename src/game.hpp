#pragma once

#include <deque>

struct Pos {
  int x = 0;
  int y = 0;
  bool operator==(const Pos&) const = default;
};

enum class Direction { Up, Down, Left, Right };

class Game {
 public:
  Game(int width, int height);

  void reset();
  // Advance one tick. Returns false if this move ended the game.
  bool step();
  void setDirection(Direction d);

  int width() const { return w_; }
  int height() const { return h_; }
  int score() const { return score_; }
  bool isGameOver() const { return gameOver_; }
  Direction direction() const { return dir_; }
  const std::deque<Pos>& snake() const { return snake_; }
  Pos food() const { return food_; }
  Pos head() const { return snake_.front(); }

  // Test hooks (no I/O, no curses).
  void setFoodForTest(Pos p) { food_ = p; }
  void setSnakeForTest(std::deque<Pos> s, Direction d) {
    snake_ = std::move(s);
    dir_ = d;
    pending_ = d;
  }

 private:
  void placeFood();
  static bool isOpposite(Direction a, Direction b);

  int w_;
  int h_;
  std::deque<Pos> snake_;
  Pos food_{0, 0};
  Direction dir_ = Direction::Right;
  Direction pending_ = Direction::Right;
  int score_ = 0;
  bool gameOver_ = false;
};
