
#include "typer.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <form.h>
#include <ncurses.h>

#include "generator.hpp"

Typer::Typer() {
  initscr();
  raw();
  noecho();
  keypad(stdscr, TRUE);

  getmaxyx(stdscr, height, width);

  colors = has_colors();
  if (!colors) {
    mvprintw(1, 1, "hey... your terminal doesn't support colors! sorry :(");
    mvprintw(3, 1, "press any key to exit");
    refresh();
    getchar();
    return;
  }

  // Colors

  start_color();
  use_default_colors();
  init_pair(1, COLOR_GREEN, -1);
  init_pair(2, COLOR_RED, -1);
}

Typer::~Typer() {
  endwin();
}

void Typer::beginGame() {
  if (!colors) return;
  if (width < 60 || height < 20) {
    mvprintw(1, 1, "hey... please resize your terminal a bit bigger! thanks!");
    mvprintw(3, 1, "press any key to exit");
    refresh();
    getchar();
    return;
  }
  bool playing = true;
  while (playing) {
    char choice = showMenu();
    playing = choice != 'q';

    if (choice == 'q') {
      break;
    }
    else if (choice == 'p') {
      Typer::TypingDifficulty settings = setDifficulty();
      if (settings.length == -1) break;
      auto [text, difficulty] = generator.generateText(settings.length, settings.words, settings.numbers, settings.punctuation);
      typePhrase(text, difficulty);
    }
  }
}

char Typer::showMenu() {
  clear();
  mvprintw(1, 1, "shelltype: typing speed test in the shell");
  mvprintw(3, 1, "[p] play");
  mvprintw(5, 1, "[q] quit");
  refresh();
  return getchar();
}

Typer::TypingDifficulty Typer::setDifficulty() {
  TypingDifficulty difficulty;
  clear();
  mvprintw(1, 1, "set length: ");
  mvprintw(3, 1, "[1] easy (~60 characters)");
  mvprintw(5, 1, "[2] medium (~90 characters)");
  mvprintw(7, 1, "[3] hard (~120 characters)");
  mvprintw(9, 1, "[4] harder (~240 characters)");
  mvprintw(11, 1, "[5] insane (~480 characters)");
  mvprintw(13, 1, "or go by words: ");
  mvprintw(15, 1, "[a] easy (10 words)");
  mvprintw(17, 1, "[b] medium (20 words)");
  mvprintw(19, 1, "[c] hard (40 words)");
  mvprintw(21, 1, "[d] harder (80 words)");
  mvprintw(23, 1, "[e] insane (160 words)");
  mvprintw(25, 1, "(press one of the above numbers to keep going, or press q to go back to the main menu)");
  refresh();

  char choice;
  do {
    choice = getchar();
  } while (std::string{"12345abcdeq"}.find(choice) == std::string::npos);

  if (choice == 'q') {
    difficulty.length = -1;
    return difficulty;
  } else if (choice == '1') {
    difficulty.length = 60;
    difficulty.words = false;
  } else if (choice == '2') {
    difficulty.length = 90;
    difficulty.words = false;
  } else if (choice == '3') {
    difficulty.length = 120;
    difficulty.words = false;
  } else if (choice == '4') {
    difficulty.length = 240;
    difficulty.words = false;
  } else if (choice == '5') {
    difficulty.length = 480;
    difficulty.words = false;
  } else if (choice == 'a') {
    difficulty.length = 10;
    difficulty.words = true;
  } else if (choice == 'b') {
    difficulty.length = 20;
    difficulty.words = true;
  } else if (choice == 'c') {
    difficulty.length = 40;
    difficulty.words = true;
  } else if (choice == 'd') {
    difficulty.length = 80;
    difficulty.words = true;
  } else if (choice == 'e') {
    difficulty.length = 160;
    difficulty.words = true;
  }

  clear();
  mvprintw(1, 1, "%d %s", difficulty.length, (difficulty.words ? "words" : "characters"));
  mvprintw(3, 1, "numbers? (y/n)");
  refresh();

  do {
    choice = getchar();
  } while (std::string{"yn"}.find(choice) == std::string::npos);
  difficulty.numbers = (choice == 'y');

  clear();
  mvprintw(1, 1, "%d %s", difficulty.length, (difficulty.words ? "words" : "characters"));
  mvprintw(3, 1, difficulty.numbers ? "numbers" : "no numbers");
  mvprintw(5, 1, "punctuation? (y/n)");
  refresh();

  do {
    choice = getchar();
  } while (std::string{"yn"}.find(choice) == std::string::npos);
  difficulty.punctuation = (choice == 'y');

  return difficulty;
}

void Typer::typePhrase(const std::string &phrase, double difficulty) {
  clear();

  std::string typed = "";

  int charsTyped = 0;
  int widthForChars = width - 2;

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  mvprintw(1, 1, "%d characters (difficulty rating %f)", phrase.size(), difficulty);

  while (charsTyped < phrase.size()) {
    // row 3: clock
    mvprintw(3, 1, "time: %.2f s", (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count()) / 1000.0);

    // row 5: running text
    for (int i = 1; i < widthForChars; i++) {
      if (i - 1 + charsTyped >= phrase.size()) {
        mvprintw(5, i, " ");
      } else {
        mvprintw(5, i, "%c", phrase[i - 1 + charsTyped]);
      }
    }

    // rows 7+: overall view
    int row = 7;
    int col = 1;
    for (int i = 0; i < phrase.size(); i++) {
      if (i < charsTyped) {
        if (phrase[i] == typed[i]) {
          attron(COLOR_PAIR(1));
          mvprintw(row, col, "%c", phrase[i]);
          attroff(COLOR_PAIR(1));
        } else {
          attron(COLOR_PAIR(2));
          mvprintw(row, col, "%c", phrase[i]);
          attroff(COLOR_PAIR(2));
        }
      } else if (i == charsTyped) {
        attron(A_BOLD);
        mvprintw(row, col, "%c", phrase[i]);
        attroff(A_BOLD);
      } else {
        mvprintw(row, col, "%c", phrase[i]);
      }
      col += 1;
      if (col == width - 1) {
        col = 1;
        row += 1;
      }
    }
    refresh();

    char c = getchar();
    if (typed == "") {
      begin = std::chrono::steady_clock::now();
    }
    typed += c;
    charsTyped += 1;
  }

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  double seconds = (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) / 1000.0;

  TypingResults results = {
    phrase,
    typed,
    difficulty,
    seconds
  };

  showStatistics(results);
}

void Typer::showStatistics(const TypingResults& results) {
  clear();
  mvprintw(1, 1, "results:");
  mvprintw(3, 1, "%d characters in %.3f seconds", results.text.size(), results.seconds);

  // Calculate CPM
  double cpm = static_cast<double>(results.text.size()) / results.seconds * 60;

  mvprintw(5, 1, "cpm: %.2f characters per minute", cpm);

  // Show errors
  int numErrors = 0;

  int row = 15;
  int col = 1;

  for (int i = 0; i < results.text.size(); i++) {
    if (results.text[i] != results.typed[i]) {
      attron(COLOR_PAIR(2));
      mvprintw(row, col, "%c", results.text[i]);
      attroff(COLOR_PAIR(2));
      numErrors++;
    } else {
      attron(COLOR_PAIR(1));
      mvprintw(row, col, "%c", results.text[i]);
      attroff(COLOR_PAIR(1));
    }
    col += 1;
    if (col == width - 1) {
      col = 1;
      row += 1;
    }
  }

  mvprintw(7, 1, "errors: %d", numErrors);
  mvprintw(9, 1, "accuracy score: %.2f", (1.0 - static_cast<double>(numErrors) / results.text.size()) * 100.0);
  mvprintw(11, 1, "final score: %.2f (CPM * difficulty * accuracy)", cpm * (1.0 - static_cast<double>(numErrors) / results.text.size()) * results.difficulty);

  mvprintw(row + 2, 1, "press '+' to continue");
  refresh();
  while (getchar() != '+');
}
