#pragma once

#include "feedback.hpp"

namespace reverse_wordle {
    Feedback compare(const Word &target, const Word &guess);
} // namespace reverse_wordle
