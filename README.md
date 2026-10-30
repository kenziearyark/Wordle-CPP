# C++ Wordle

A command-line Wordle game written in C++. The game downloads a random five-letter word, accepts guesses from the player, and gives hints about correct and misplaced letters.

This project was made to practice:

- C++ classes and access control
- HTTP requests with libcurl
- JSON parsing with nlohmann/json
- String validation and transformation
- Error handling with exceptions

## How it works

When the game starts, it requests a random five-letter word from the [Random Word API](https://random-word-api.herokuapp.com/).

The player then has up to six attempts to guess the word. After an incorrect guess, the game reports whether each letter:

- Is in the correct position
- Exists in the word but is in the wrong position
- Does not exist in the word

Guesses must contain exactly five letters.

## Requirements

To build the project, you need:

- A C++17-compatible compiler
- [libcurl](https://curl.se/libcurl/)
- [nlohmann/json](https://github.com/nlohmann/json)
- An internet connection while playing

## Building

If the source file is named `main.cpp`, compile it with:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -lcurl -o wordle
```

## Possible future improvements

- Add colored terminal output similar to the original Wordle
- Allow the player to start another round
- Keep track of wins and statistics
- Add an offline word list when the API is unavailable
- Verify guesses against a dictionary
- Split the class into header and source files

## Acknowledgements

Random words are provided by the [Random Word API](https://random-word-api.herokuapp.com/).
