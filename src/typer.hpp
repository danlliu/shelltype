// typer.hpp

#ifndef TYPER_HPP
#define TYPER_HPP

#include <string>
#include <ncurses.h>

#include "generator.hpp"

class Typer {
 public:
  Typer();
  ~Typer();

  Typer(Typer& other) = delete;
  Typer& operator=(Typer& other) = delete;
  Typer(Typer&& other) = delete;
  Typer& operator=(Typer&& other) = delete;

  void beginGame();

 private:
  struct TypingDifficulty {
    int length;
    bool words;
    bool numbers;
    bool punctuation;
  };

  struct TypingResults {
    std::string text;
    std::string typed;
    double difficulty;

    double seconds;
  };

  char showMenu();
  TypingDifficulty setDifficulty();
  void typePhrase(const std::string& text, double difficulty);
  void showStatistics(const TypingResults& results);

  TextGenerator generator;
  int width, height;
  bool colors;
};

#endif
