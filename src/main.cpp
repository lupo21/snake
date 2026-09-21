#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>

#include "game.hpp"
#include "term.hpp"

namespace {
struct Args {
  int width = 20;
  int height = 12;
  int fps = 10;
};

Args parseArgs(int argc, char** argv) {
  Args a;
  for (int i = 1; i < argc; ++i) {
    std::string s = argv[i];
    auto val = [&](int& out) {
      if (i + 1 < argc) out = std::stoi(argv[++i]);
    };
    if (s == "--width") val(a.width);
    else if (s == "--height") val(a.height);
    else if (s == "--fps") val(a.fps);
    else if (s == "--help" || s == "-h") {
      std::puts("snake --width 20 --height 12 --fps 10\n"
                "Keys: arrows/WASD move, p/space pause, r restart, q quit");
      std::exit(0);
    }
  }
  if (a.width < 6) a.width = 6;
  if (a.height < 6) a.height = 6;
  if (a.fps < 1) a.fps = 1;
  if (a.fps > 60) a.fps = 60;
  return a;
}

void render(const Game& g, bool paused) {
  // Home cursor + erase to end of screen so shorter frames (e.g. after the
  // GAME OVER line disappears on restart) leave no residue.
  std::fputs("\x1b[H\x1b[J", stdout);
  std::printf("Score: %d  (q quit, p pause, r restart)\n", g.score());
  std::putchar('+');
  for (int x = 0; x < g.width(); ++x) std::putchar('-');
  std::puts("+");
  const auto& snake = g.snake();
  const Pos food = g.food();
  for (int y = 0; y < g.height(); ++y) {
    std::putchar('|');
    for (int x = 0; x < g.width(); ++x) {
      Pos p{x, y};
      char ch = ' ';
      if (p == food) ch = '*';
      for (size_t i = 0; i < snake.size(); ++i) {
        if (snake[i] == p) {
          ch = (i == 0) ? '@' : 'o';
          break;
        }
      }
      std::putchar(ch);
    }
    std::puts("|");
  }
  std::putchar('+');
  for (int x = 0; x < g.width(); ++x) std::putchar('-');
  std::puts("+");
  if (paused) std::puts("-- PAUSED (p to resume) --");
  if (g.isGameOver())
    std::puts("GAME OVER — press r to restart, q to quit");
  std::fflush(stdout);
}
}  // namespace

int main(int argc, char** argv) {
  Args args = parseArgs(argc, argv);
  RawTerminal term;  // Restores terminal + cursor on exit.
  std::fputs("\x1b[2J", stdout);  // One full clear at startup.

  Game game(args.width, args.height);
  bool paused = false;
  render(game, paused);

  auto last = std::chrono::steady_clock::now();
  while (true) {
    // Drain all pending keys each frame so fast input isn't dropped.
    while (true) {
      const Key k = pollKey();
      if (k == Key::None) break;
      switch (k) {
        case Key::Quit: return 0;
        case Key::Pause:
          if (!game.isGameOver()) {
            paused = !paused;
            render(game, paused);
          }
          break;
        case Key::Restart:
          game.reset();
          paused = false;
          last = std::chrono::steady_clock::now();
          render(game, paused);
          break;
        case Key::Up: game.setDirection(Direction::Up); break;
        case Key::Down: game.setDirection(Direction::Down); break;
        case Key::Left: game.setDirection(Direction::Left); break;
        case Key::Right: game.setDirection(Direction::Right); break;
        case Key::None: break;
      }
    }

    if (!paused && !game.isGameOver()) {
      // Speed up slightly with score; clamp to avoid unplayable rates.
      const int intervalMs =
          std::max(40, 1000 / args.fps - game.score() * 2);
      const auto now = std::chrono::steady_clock::now();
      if (now - last >= std::chrono::milliseconds(intervalMs)) {
        last = now;
        game.step();
        render(game, paused);
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
      }
    } else {
      // Paused or game over: static screen, redraw only on transitions
      // (pause/restart keys and step() render explicitly above).
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
  }
  return 0;
}
