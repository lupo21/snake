#pragma once

// Minimal POSIX terminal helper: raw mode + non-blocking key parse.
// No ncurses dependency; uses termios + ANSI escape codes.

enum class Key {
  None,
  Up,
  Down,
  Left,
  Right,
  Quit,     // q, Q, Ctrl-C
  Pause,    // p, P, space
  Restart,  // r, R
};

class RawTerminal {
 public:
  RawTerminal();
  ~RawTerminal();
  RawTerminal(const RawTerminal&) = delete;
  RawTerminal& operator=(const RawTerminal&) = delete;

 private:
  bool active_ = false;
};

// Non-blocking: returns Key::None when no input is pending.
Key pollKey();
