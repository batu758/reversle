# reversle

`reversle` is a reverse Wordle solver written in C++. Given one or more sequences of Wordle feedback in hard mode, it determines target words for which a consistent sequence of guesses exists, and can construct a such sequence accordingly.

The project explores how the puzzle can be formulated as a compact finite-state reachability problem rather than brute-forcing all allowed guesses with exponential complexity.

## Implementation

For a fixed target word, the solver represents the information discovered so far using a 5-bit state:

- each bit corresponds to a position in the target word
- a set bit indicates that the corresponding target letter has already been discovered
- there are therefore only 2^5 = 32 possible states.

For each allowed guess, the solver determines the Wordle feedback and the target positions whose letters occur in the guess. This defines transitions towards which of the 32 states are possible. Transitions compatible with the hard-mode constraints are also precomputed (previously known letters should be a subset of newly known letters). Together they define allowed transitions between the 32 states.

Then a feedback sequence can then be checked as a finite-state reachability problem over these states. For target words for which a solution exists, the solver reconstructs one corresponding sequence of guesses by backtracking over reachable states.

Wordle feedback is represented using three possible values per position (green, yellow, or gray), giving 3^5 = 243 possible feedback patterns. Five-letter words and knowledge states use compact representations that allow efficient bitwise operations for evaluating wordle comparisons and propagating transitions.

With the word length and transition tables fixed, processing a feedback sequence takes linear time in the length of the sequence.

## Building

The project requires:
- CMake 3.28 or later
- A C++23-compatible compiler

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The project uses [CLI11](https://github.com/CLIUtils/CLI11) for command-line argument parsing.

## Examples

Given a sequence of feedback such as:

`__Y__/_YGYY/_GGYG/GGGGG`

the solver can search the target-word dictionary for target words for which the sequence is achievable, and construct a corresponding sequence of guesses.

### Reconstructing Guesses (Default):

```bash
./reversle "__Y__/_YGYY/_GGYG/GGGGG"
```

Example output:

```
Possible target word: shake
-----
bield
deash
chase
shake
-----
```

### Multiple sequences:

```bash
./reversle "YYYYY/GYYYY/GGYYY/GGGYY/GGGGG" "YYYYY/YYGYY/GGGGG"
```

Example output:

```
Possible target word: steal
-----
leats
salet
stale
stela
steal
-----
leats
taels
steal
-----
```

### Checking satisfiability:

```bash
./reversle --check-sat "__Y__/_YGYY/_GGYG/GGGGG"
```

Which outputs `sat` or `unsat`.

### Listing possible target words:

```bash
./reversle --list-possible "__Y__/_YGYY/_GGYG/GGGGG"
```

Example output:

```
Listing possible target words:
-----
shake
leapt
...
share
-----
eliminated 2240 words
total 75 possible words
```

### Overriding word lists:

By default the program looks for these files: `official_allowed_guesses.txt` and `shuffled_real_wordles.txt`. Custom word lists can be supplied with:
```bash
./reversle \
    --possible-words path/to/targets.txt \
    --allowed-guesses path/to/guesses.txt \
    "__Y__/_YGYY/_GGYG/GGGGG"
```
