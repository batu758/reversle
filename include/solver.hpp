#pragma once

#include "feedback.hpp"
#include "word.hpp"

#include <vector>

namespace reverse_wordle {
    bool is_target_satisfiable(
        const Word &target,
        const std::vector<Word> &allowed_guesses,
        const std::vector<std::vector<Feedback>> &sequences
    );

    std::vector<Word> filter_target_words(
        const std::vector<Word> &possible_targets,
        const std::vector<Word> &allowed_guesses,
        const std::vector<std::vector<Feedback>> &sequences
    );

    std::vector<Word> find_any_solution_for_target(
        const Word &target,
        const std::vector<Word> &allowed_guesses,
        const std::vector<Feedback> &sequence
    );
} // namespace reverse_wordle
