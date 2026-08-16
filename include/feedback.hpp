#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace reverse_wordle {
    constexpr std::size_t FEEDBACK_COMBINATIONS = 3 * 3 * 3 * 3 * 3;

    class Word;

    enum class LetterFeedback : std::uint8_t {
        Green,
        Gray,
        Yellow
    };

    class Feedback {
    public:
        static Feedback from_string(std::string_view value);
        static Feedback from_value(std::uint8_t value) noexcept;

        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] std::uint8_t value() const noexcept;

        [[nodiscard]] LetterFeedback operator[](const std::size_t position) const noexcept {
            return static_cast<LetterFeedback>((value_ / pow3(position)) % 3);
        }

        friend bool operator==(const Feedback &, const Feedback &) = default;

    private:
        explicit Feedback(const std::uint8_t value) : value_(value) {}

        static constexpr std::uint8_t pow3(const std::size_t position) noexcept {
            std::uint8_t result = 1;
            for (std::size_t i = 0; i < position; ++i)
                result *= 3;
            return result;
        }

        std::uint8_t value_;
    };
} // namespace reverse_wordle
