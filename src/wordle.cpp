#include "wordle.hpp"
#include "word.hpp"

#include <cstdint>

namespace reverse_wordle {
    Feedback compare(const Word &target, const Word &guess) {
        static int8_t counts[256] = {};

        std::uint8_t state[WORD_LENGTH];

        for (std::size_t i = 0; i < WORD_LENGTH; ++i)
            counts[guess[i]] = 0;

        // 0 = different, 1 = equal
        for (std::size_t i = 0; i < WORD_LENGTH; ++i) {
            const auto target_letter = target[i];
            const auto guess_letter = guess[i];

            const auto equal = target_letter == guess_letter;
            state[i] = equal;
            counts[target_letter] += equal ? 0 : 1;
        }

        // Mark letter colors
        for (std::size_t i = 0; i < WORD_LENGTH; ++i) {
            if (!state[i] && counts[guess[i]]-- > 0)
                state[i] = static_cast<std::uint8_t>(LetterFeedback::Yellow);
            else if (state[i])
                state[i] = static_cast<std::uint8_t>(LetterFeedback::Green);
            else
                state[i] = static_cast<std::uint8_t>(LetterFeedback::Gray);
        }

        std::uint8_t encoded = 0;
        for (std::size_t i = WORD_LENGTH; i-- > 0;)
            encoded = encoded * 3 + state[i];

        return Feedback::from_value(encoded);
    }
} // namespace reverse_wordle
