// generator.hpp

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <random>
#include <string>
#include <utility>
#include <vector>

class TextGenerator {
 public:
  TextGenerator();

  std::string nextWord();
  std::pair<std::string, double> generateText(int length, bool countingWords, bool numbers, bool punctuation);
 private:
  static const std::string nounsFile;
  static const std::string verbsFile;
  static const std::string adjectivesFile;

  std::vector<std::string> nounsList;
  std::vector<std::string> verbsList;
  std::vector<std::string> adjectivesList;

  std::minstd_rand generator;

  enum WordType {
    NONE,
    NOUN,
    VERB,
    ADJECTIVE
  };
  WordType lastWordType = WordType::NONE;
};

#endif
