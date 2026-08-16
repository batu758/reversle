#include "reader.hpp"
#include "solver.hpp"
#include "wordle.hpp"

#include <CLI/CLI.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace reverse_wordle {
    void print_any_solution(
        const std::vector<Word> &possible_targets,
        const std::vector<Word> &allowed_guesses,
        const std::vector<std::vector<Feedback>> &sequences
    ) {
        for (const auto &target : possible_targets) {
            if (is_target_satisfiable(target, allowed_guesses, sequences)) {
                std::cout << "Possible target word: " << target.to_string() << "\n";
                std::cout << "-----\n";
                for (const auto &sequence : sequences) {
                    for (const auto &word : find_any_solution_for_target(target, allowed_guesses, sequence)) {
                        std::cout << word.to_string() << "\n";
                    }
                    std::cout << "-----\n";
                }
                return;
            }
        }
        std::cout << "unsat\n";
    }

    void print_satisfiable(
        const std::vector<Word> &possible_targets,
        const std::vector<Word> &allowed_guesses,
        const std::vector<std::vector<Feedback>> &sequences
    ) {
        for (const auto &target : possible_targets) {
            if (is_target_satisfiable(target, allowed_guesses, sequences)) {
                std::cout << "sat\n";
                return;
            }
        }
        std::cout << "unsat\n";
    }

    void list_possible_targets(
        const std::vector<Word> &possible_targets,
        const std::vector<Word> &allowed_guesses,
        const std::vector<std::vector<Feedback>> &sequences
    ) {
        const auto filtered = filter_target_words(possible_targets, allowed_guesses, sequences);

        std::cout << "Listing possible target words:\n";
        std::cout << "-----\n";
        for (const auto &target : filtered) {
            std::cout << target.to_string() << "\n";
        }
        std::cout << "-----\n";
        std::cout << "eliminated " << possible_targets.size() - filtered.size() << " words\n";
        std::cout << "total " << filtered.size() << " possible words\n";
    }

    bool check_sequences(const std::vector<std::vector<Feedback>> &sequences) {
        for (const auto &sequence : sequences) {
            for (std::size_t i = 1; i < sequence.size(); ++i) {
                const auto &prev = sequence[i - 1];
                const auto &cur = sequence[i];

                // Green letters must remain in the same position.
                for (std::size_t position = 0; position < WORD_LENGTH; ++position) {
                    if (
                        prev[position] == LetterFeedback::Green &&
                        cur[position] != prev[position]
                    ) {
                        std::cout <<
                            "Invalid input: Green letters should remain in the same place in a hard mode play.\n";
                        std::cout << prev.to_string() << "\n";
                        std::cout << cur.to_string() << "\n";
                        for (int p = 0; p < position; ++p)
                            std::cout << " ";
                        std::cout << "^\n";
                        return false;
                    }
                }

                // If the previous guess had yellow letters,
                // the current guess should have more green letters or at least a yellow letter
                int prev_yellows = 0;
                int prev_greens = 0;
                for (std::size_t position = 0; position < WORD_LENGTH; ++position) {
                    prev_yellows += prev[position] == LetterFeedback::Yellow;
                    prev_greens += prev[position] == LetterFeedback::Green;
                }

                if (prev_yellows > 0) {
                    int cur_yellows = 0;
                    int cur_greens = 0;
                    for (std::size_t position = 0; position < WORD_LENGTH; ++position) {
                        cur_yellows += cur[position] == LetterFeedback::Yellow;
                        cur_greens += cur[position] == LetterFeedback::Green;
                    }

                    if (cur_yellows == 0 && cur_greens <= prev_greens) {
                        std::cout << "Invalid input: Yellow letters cannot be discarded in a hard mode play.\n";
                        std::cout << prev.to_string() << "\n";
                        std::cout << cur.to_string() << "\n";
                        return false;
                    }
                }
            }
        }

        return true;
    }

    std::vector<std::vector<Feedback>> parse_plays(const std::vector<std::string> &plays) {
        std::vector<std::vector<Feedback>> sequences;
        sequences.reserve(plays.size());

        for (const auto &play : plays) {
            std::vector<Feedback> sequence;

            std::size_t start = 0;
            while (start < play.size()) {
                const std::size_t end = play.find('/', start);

                const std::size_t length =
                    end == std::string::npos
                        ? play.size() - start
                        : end - start;

                sequence.push_back(Feedback::from_string(play.substr(start, length)));

                if (end == std::string::npos)
                    break;

                start = end + 1;
            }

            sequences.push_back(std::move(sequence));
        }

        return sequences;
    }
} // namespace reverse_wordle

int main(int argc, char **argv) {
    using namespace reverse_wordle;

    CLI::App app{"reversle"};

    std::vector<std::string> plays;
    std::string possible_targets_file = "shuffled_real_wordles.txt";
    std::string allowed_guesses_file = "official_allowed_guesses.txt";

    bool check_sat = false;
    bool list_possible = false;

    app.add_option(
        "plays",
        plays,
        "Feedback from hard mode wordle plays to process.\n"
        "Examples:\n"
        "\t__Y__/Y__YY/_GYGG/GGGGG\n"
        "\tY_Y__/YGG_Y/GGG_G"
    )->expected(-1);

    app.add_option(
        "--possible-words",
        possible_targets_file,
        "File containing possible target words"
    )->check(CLI::ExistingFile)->capture_default_str();

    app.add_option(
        "--allowed-guesses",
        allowed_guesses_file,
        "File containing allowed guesses"
    )->check(CLI::ExistingFile)->capture_default_str();

    auto *check_sat_option = app.add_flag(
        "--check-sat",
        check_sat,
        "Check whether the plays are satisfiable"
    );

    auto *list_possible_option = app.add_flag(
        "--list-possible",
        list_possible,
        "List all possible target words"
    );

    check_sat_option->excludes(list_possible_option);
    list_possible_option->excludes(check_sat_option);

    CLI11_PARSE(app, argc, argv);

    try {
        const std::vector<std::vector<Feedback>> sequences = parse_plays(plays);

        if (sequences.empty()) {
            std::cout << "No plays were given.\n";
            return 0;
        }

        if (!check_sequences(sequences))
            return 1;

        const auto possible_targets = read_words(possible_targets_file);
        const auto allowed_guesses = read_words(allowed_guesses_file);

        if (check_sat) {
            print_satisfiable(possible_targets, allowed_guesses, sequences);
        } else if (list_possible) {
            list_possible_targets(possible_targets, allowed_guesses, sequences);
        } else {
            print_any_solution(possible_targets, allowed_guesses, sequences);
        }

        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
