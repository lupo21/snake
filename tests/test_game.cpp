// Dependency-free test runner: `ctest` runs this binary, non-zero = fail.
#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "game.hpp"

namespace {
int g_pass = 0;
void check(bool cond, const char* name) {
  if (!cond) {
    std::printf("FAIL: %s\n", name);
    std::exit(1);
  }
  ++g_pass;
  std::printf("ok: %s\n", name);
}
}  // namespace

int main() {
  {
    Game g(10, 10);
    const Pos h0 = g.head();
    g.step();
    check(g.head().x == h0.x + 1 && g.head().y == h0.y, "moves right");
    check(g.snake().size() == 3, "keeps length without food");
  }
  {
    Game g(10, 10);
    g.setDirection(Direction::Left);  // 180 reversal must be ignored
    Pos h0 = g.head();
    g.step();
    check(g.head().x == h0.x + 1, "ignores reverse");
  }
  {
    Game g(10, 10);
    Pos h = g.head();
    g.setFoodForTest({h.x + 1, h.y});
    const size_t n = g.snake().size();
    g.step();
    check(g.snake().size() == n + 1, "grows on food");
    check(g.score() == 1, "scores on food");
  }
  {
    Game g(6, 6);
    // Drive into the right wall.
    for (int i = 0; i < 10 && !g.isGameOver(); ++i) g.step();
    check(g.isGameOver(), "wall collision ends game");
  }
  {
    Game g(10, 10);
    // Head at (5,5) moving left into its own body at (4,5).
    g.setSnakeForTest({{5, 5}, {4, 5}, {4, 6}, {5, 6}}, Direction::Left);
    g.setFoodForTest({0, 0});
    g.step();
    check(g.isGameOver(), "self collision ends game");
  }
  {
    Game g(10, 10);
    g.setDirection(Direction::Down);
    g.step();
    check(g.direction() == Direction::Down, "turn commits");
  }
  std::printf("all %d tests passed\n", g_pass);
  return 0;
}
