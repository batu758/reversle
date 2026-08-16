#include "feedback.hpp"
#include "word.hpp"

#include <stdexcept>

namespace reverse_wordle {
    namespace {
        LetterFeedback from_char(const char value) {
            switch (value) {
            case 'G': return LetterFeedback::Green;
            case '_': return LetterFeedback::Gray;
            case 'Y': return LetterFeedback::Yellow;
            default:
                throw std::invalid_argument("Invalid feedback character");
            }
        }

        char to_char(const LetterFeedback value) {
            switch (value) {
            case LetterFeedback::Green: return 'G';
            case LetterFeedback::Gray: return '_';
            case LetterFeedback::Yellow: return 'Y';
            }

            return '?';
        }
    } // namespace

    Feedback Feedback::from_string(const std::string_view value) {
        if (value.size() != WORD_LENGTH)
            throw std::invalid_argument("Feedback must contain exactly 5 characters");

        std::uint8_t encoded = 0;

        for (std::size_t i = value.size(); i-- > 0;)
            encoded = encoded * 3 +
                static_cast<std::uint8_t>(from_char(value[i]));

        return Feedback(encoded);
    }

    Feedback Feedback::from_value(const std::uint8_t value) noexcept {
        return Feedback(value);
    }

    std::string Feedback::to_string() const {
        std::string result(WORD_LENGTH, '_');

        for (std::size_t i = 0; i < WORD_LENGTH; ++i)
            result[i] = to_char((*this)[i]);

        return result;
    }

    std::uint8_t Feedback::value() const noexcept {
        return value_;
    }
} // namespace reverse_wordle
