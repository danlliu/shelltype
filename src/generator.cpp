
#include "generator.hpp"

#include <array>
#include <fstream>
#include <random>
#include <string>
#include <utility>
#include <vector>

const std::string TextGenerator::nounsFile = "data/nouns.txt";
const std::string TextGenerator::verbsFile = "data/verbs.txt";
const std::string TextGenerator::adjectivesFile = "data/adjectives.txt";

TextGenerator::TextGenerator() {
  generator.seed(time(nullptr));

  std::ifstream nouns(nounsFile);
  std::ifstream verbs(verbsFile);
  std::ifstream adjectives(adjectivesFile);
  std::string line;
  while (std::getline(nouns, line)) {
    nounsList.push_back(line);
  }
  while (std::getline(verbs, line)) {
    verbsList.push_back(line);
  }
  while (std::getline(adjectives, line)) {
    adjectivesList.push_back(line);
  }
  nouns.close();
  verbs.close();
  adjectives.close();
}

std::string TextGenerator::nextWord() {
  std::string word;
  auto gen = std::uniform_int_distribution(0, 2);
  int choice = gen(generator);
  if (lastWordType == WordType::NONE) {
    switch(choice) {
      case 0:
        word = nounsList[std::uniform_int_distribution(0ul, nounsList.size() - 1)(generator)];
        lastWordType = WordType::NOUN;
        break;
      case 1:
        word = verbsList[std::uniform_int_distribution(0ul, verbsList.size() - 1)(generator)];
        lastWordType = WordType::VERB;
        break;
      case 2:
        word = adjectivesList[std::uniform_int_distribution(0ul, adjectivesList.size() - 1)(generator)];
        lastWordType = WordType::ADJECTIVE;
        break;
      default:
        word = "<ERROR>";
        break;
    }
  }
  gen = std::uniform_int_distribution(0, 1);
  choice = gen(generator);
  if (choice == 0 && (lastWordType == WordType::VERB || lastWordType == WordType::ADJECTIVE)) {
    word = nounsList[std::uniform_int_distribution(0ul, nounsList.size() - 1)(generator)];
    lastWordType = WordType::NOUN;
  } else if ((choice == 1 && lastWordType == WordType::NOUN) || (choice == 0 && lastWordType == WordType::ADJECTIVE)) {
    word = verbsList[std::uniform_int_distribution(0ul, verbsList.size() - 1)(generator)];
    lastWordType = WordType::VERB;
  } else {
    word = adjectivesList[std::uniform_int_distribution(0ul, adjectivesList.size() - 1)(generator)];
    lastWordType = WordType::ADJECTIVE;
  }
  return word;
}

std::pair<std::string, double> TextGenerator::generateText(int length, bool countingWords, bool numbers, bool punctuation) {
  std::string text;
  int numWords = 0;
  double difficulty = 0.0;
  do {
    int gen = std::uniform_int_distribution(0, 9)(generator);
    numWords++;
    if (gen < 3 && numbers) {
      std::string num = std::to_string(std::uniform_int_distribution(-999999, 999999)(generator));
      text += num;
      difficulty += num.size();
      if (num.find_first_of("-") != std::string::npos) {
        difficulty += 0.5;
      }

      if (!countingWords && text.size() >= length) {
        break;
      }
      if (countingWords && numWords >= length) {
        break;
      }
      text += " ";
    } else {
      std::string word = nextWord();
      text += word;
      difficulty += word.size() / 30.0;
      if (word.find_first_of("-") != std::string::npos) {
        difficulty += 0.5;
      }

      if (!countingWords && text.size() >= length) {
        break;
      }
      if (countingWords && numWords >= length) {
        break;
      }
      if (gen <= 7 && punctuation) {
        // random punctuation character
        const std::array<std::string, 6> punctuation = {".", ",", "!", "?", ";", ":"};
        text += punctuation[std::uniform_int_distribution(0ul, punctuation.size() - 1)(generator)];
        difficulty += 1.0;
      }
      text += " ";
    }
  } while (!countingWords || numWords < length);
  return {text, difficulty};
}
