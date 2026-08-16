#include "reader.hpp"

#include <fstream>
#include <stdexcept>

namespace reverse_wordle {
    std::vector<Word> read_words(const std::string &filename) {
        std::ifstream file(filename);

        if (!file)
            throw std::runtime_error("Failed to open word file: " + filename);

        std::vector<Word> words;
        std::string value;

        while (file >> value)
            words.push_back(Word::from_string(value));

        if (file.bad())
            throw std::runtime_error("Failed while reading word file: " + filename);

        return words;
    }
} // namespace reverse_wordle
