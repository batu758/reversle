#include "solver.hpp"
#include "wordle.hpp"

#include <array>
#include <algorithm>
#include <cstdint>
#include <span>

namespace reverse_wordle {
    namespace {
        constexpr std::array<std::uint32_t, 32> VALID_TRANSITIONS_PER_KNOWN_LETTERS = [] {
            std::array<std::uint32_t, 32> transitions{};

            for (std::uint8_t known_letters = 0; known_letters < 32; ++known_letters) {
                for (std::uint8_t next = 0; next < 32; ++next) {
                    // A transition is valid if all previously known letters are still known.
                    if ((next & known_letters) == known_letters)
                        transitions[known_letters] |= 1u << next;
                }
            }

            return transitions;
        }();

        struct TargetTransitions {
            std::array<std::uint8_t, 256> flag_per_letter{};
            std::array<std::uint32_t, FEEDBACK_COMBINATIONS> possible_states_per_feedback{};
        };

        TargetTransitions build_target_transitions(
            const Word &target,
            const std::span<const Word> allowed_guesses
        ) {
            TargetTransitions transitions;

            for (std::size_t position = 0; position < WORD_LENGTH; ++position)
                transitions.flag_per_letter[target[position]] = 1u << position;

            for (const auto &guess : allowed_guesses) {
                const auto feedback = compare(target, guess);

                // Represents the set of letters inside the target that were guessed.
                // Each bit stores the position of a known letter in the target.
                std::uint8_t letters = 0;

                for (std::size_t position = 0; position < WORD_LENGTH; ++position)
                    letters |= transitions.flag_per_letter[guess[position]];

                transitions.possible_states_per_feedback[feedback.value()] |= 1u << letters;
            }

            return transitions;
        }

        bool is_feedback_sequence_possible(
            const std::span<const Feedback> sequence,
            const std::span<const std::uint32_t> possible_states_per_feedback
        ) {
            // Only 0b00000 (no letters are known) is possible at the start.
            // It corresponds to the least significant bit.
            std::uint32_t reachable_states = 1;

            for (const auto &feedback : sequence) {
                const std::uint32_t possible_states = possible_states_per_feedback[feedback.value()];

                std::uint32_t next_reachable_states = 0;

                for (std::uint8_t known_letters = 0; known_letters < 32; ++known_letters) {
                    if ((reachable_states >> known_letters) & 1)
                        next_reachable_states |= VALID_TRANSITIONS_PER_KNOWN_LETTERS[known_letters] & possible_states;
                }

                reachable_states = next_reachable_states;
            }

            // No valid set of known letters remains if reachable_states is 0.
            return reachable_states != 0;
        }
    } // namespace

    bool is_target_satisfiable(
        const Word &target,
        const std::vector<Word> &allowed_guesses,
        const std::vector<std::vector<Feedback>> &sequences
    ) {
        const auto transitions = build_target_transitions(target, allowed_guesses);

        for (const auto &sequence : sequences) {
            if (!is_feedback_sequence_possible(sequence, transitions.possible_states_per_feedback))
                return false;
        }

        return true;
    }

    std::vector<Word> filter_target_words(
        const std::vector<Word> &possible_targets,
        const std::vector<Word> &allowed_guesses,
        const std::vector<std::vector<Feedback>> &sequences
    ) {
        std::vector<Word> filtered;

        for (const auto &target : possible_targets) {
            if (is_target_satisfiable(target, allowed_guesses, sequences))
                filtered.push_back(target);
        }

        return filtered;
    }

    std::vector<Word> find_any_solution_for_target(
        const Word &target,
        const std::vector<Word> &allowed_guesses,
        const std::vector<Feedback> &sequence
    ) {
        const auto transitions = build_target_transitions(target, allowed_guesses);

        std::vector<std::uint32_t> reachable_states;
        reachable_states.reserve(sequence.size() + 1);
        reachable_states.push_back(1);

        for (const auto &feedback : sequence) {
            const std::uint32_t possible_states = transitions.possible_states_per_feedback[feedback.value()];

            std::uint32_t next_reachable_states = 0;

            for (std::uint8_t known_letters = 0; known_letters < 32; ++known_letters) {
                if ((reachable_states.back() >> known_letters) & 1)
                    next_reachable_states |= VALID_TRANSITIONS_PER_KNOWN_LETTERS[known_letters] & possible_states;
            }

            reachable_states.push_back(next_reachable_states);
        }

        if (reachable_states.back() == 0)
            return {};

        std::vector<Word> solution;
        solution.reserve(sequence.size());

        for (auto i = sequence.size(); i-- > 0;) {
            for (const auto &guess : allowed_guesses) {
                if (compare(target, guess) != sequence[i])
                    continue;

                std::uint8_t letters = 0;

                for (std::size_t position = 0; position < WORD_LENGTH; ++position)
                    letters |= transitions.flag_per_letter[guess[position]];

                if (!((1u << letters) & reachable_states[i + 1]))
                    continue;

                if (i < sequence.size() - 1 &&
                    !(VALID_TRANSITIONS_PER_KNOWN_LETTERS[letters] & reachable_states[i + 2]))
                    continue;

                solution.push_back(guess);
                reachable_states[i + 1] = 1u << letters;
                break;
            }
        }

        std::ranges::reverse(solution);
        return solution;
    }
} // namespace reverse_wordle
