#pragma once

#include "word.hpp"

#include <string>
#include <vector>

namespace reverse_wordle {
    std::vector<Word> read_words(const std::string &filename);
} // namespace reverse_wordle
