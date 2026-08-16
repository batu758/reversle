#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace reverse_wordle {
    constexpr std::size_t WORD_LENGTH = 5;

    class Word {
    public:
        static Word from_string(std::string_view value);

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] char operator[](const std::size_t position) const noexcept {
            return static_cast<char>((value_ >> (position * 8)) & 0xff);
        }

        friend bool operator==(const Word &, const Word &) = default;

    private:
        explicit Word(const std::uint64_t value) : value_(value) {}

        std::uint64_t value_;
    };
} // namespace reverse_wordle
