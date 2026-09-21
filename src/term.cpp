#include "term.hpp"

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <cstdio>

namespace {
termios g_orig{};
bool g_saved = false;
}  // namespace

RawTerminal::RawTerminal() {
  if (!isatty(STDIN_FILENO)) return;
  if (tcgetattr(STDIN_FILENO, &g_orig) != 0) return;
  g_saved = true;
  termios raw = g_orig;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  // NOTE: keep OPOST/ONLCR enabled. stdin/stdout share the same tty, so
  // clearing OPOST here would stop '\n' from returning to column 0 and the
  // board would render as a staircase.
  raw.c_oflag |= (OPOST | ONLCR);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  active_ = true;
  // Hide cursor.
  std::fputs("\x1b[?25l", stdout);
  std::fflush(stdout);
}

RawTerminal::~RawTerminal() {
  if (active_ && g_saved) {
    std::fputs("\x1b[?25h", stdout);
    std::fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_orig);
  }
}

namespace {
bool inputReady() {
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(STDIN_FILENO, &rfds);
  timeval tv{0, 0};
  return select(STDIN_FILENO + 1, &rfds, nullptr, nullptr, &tv) > 0;
}
}  // namespace

Key pollKey() {
  if (!inputReady()) return Key::None;
  char c = 0;
  if (read(STDIN_FILENO, &c, 1) != 1) return Key::None;

  if (c == '\x1b') {
    // Escape sequence: expect "[A/B/C/D". Non-blocking: bail if incomplete.
    if (!inputReady()) return Key::None;
    char b = 0;
    if (read(STDIN_FILENO, &b, 1) != 1 || b != '[') return Key::None;
    if (!inputReady()) return Key::None;
    char d = 0;
    if (read(STDIN_FILENO, &d, 1) != 1) return Key::None;
    switch (d) {
      case 'A':
        return Key::Up;
      case 'B':
        return Key::Down;
      case 'C':
        return Key::Right;
      case 'D':
        return Key::Left;
      default:
        return Key::None;
    }
  }
  switch (c) {
    case 'q':
    case 'Q':
    case '\x03':
      return Key::Quit;  // Ctrl-C in raw mode arrives as 0x03
    case 'p':
    case 'P':
    case ' ':
      return Key::Pause;
    case 'r':
    case 'R':
      return Key::Restart;
    case 'w':
    case 'W':
      return Key::Up;
    case 's':
    case 'S':
      return Key::Down;
    case 'a':
    case 'A':
      return Key::Left;
    case 'd':
    case 'D':
      return Key::Right;
    default:
      return Key::None;
  }
}
