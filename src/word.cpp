#include "word.hpp"

#include <stdexcept>

namespace reverse_wordle {
    Word Word::from_string(const std::string_view value) {
        if (value.size() != WORD_LENGTH)
            throw std::invalid_argument("Word must contain exactly 5 letters");

        std::uint64_t packed = 0;

        for (std::size_t i = 0; i < WORD_LENGTH; ++i)
            packed |= static_cast<std::uint64_t>(
                static_cast<unsigned char>(value[i])
            ) << (i * 8);

        return Word(packed);
    }

    std::string Word::to_string() const {
        std::string result(WORD_LENGTH, '\0');

        for (std::size_t i = 0; i < WORD_LENGTH; ++i)
            result[i] = (*this)[i];

        return result;
    }
} // namespace reverse_wordle
