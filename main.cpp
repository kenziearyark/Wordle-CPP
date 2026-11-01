#include <cctype>
#include <cstdint>
#include <algorithm>
#include <cstdio>
#include <string>
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>

class gameMain {
private:
    int8_t correct[5] = {0};

    static std::string suffix(int num) {
        if (num == 1) {return "st";}
        else if (num == 2) {return "nd";}
        else if (num == 3) {return "rd";}
        else {return "th";}
    }

    static void makeUppercase(std::string& g) {
        std::transform(g.begin(), g.end(), g.begin(), [](unsigned char c){
            return toupper(c);
        });
    }

    // WORD FINDER
    static std::size_t writeResponse(
        char* data,
        std::size_t size,
        std::size_t count,
        void* output
    ) {
        const std::size_t bytes = size * count;
        static_cast<std::string*>(output)->append(data, bytes);
        return bytes;
    }
    std::string getWord() {
        CURL* curl = curl_easy_init();
        if (curl == nullptr) {
            throw std::runtime_error("Could not initialize curl");
        }

        // Guard to ensure curl cleanup happens automatically on return or exception
        struct CurlGuard {
            CURL* handle;
            ~CurlGuard() { if (handle) curl_easy_cleanup(handle); }
        } guard{curl};

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, "https://random-word-api.herokuapp.com/word?length=5");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

        const CURLcode result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(result));
        }

        // Safeguard against malformed JSON or empty API responses
        try {
            const nlohmann::json json = nlohmann::json::parse(response);
            if (json.is_array() && !json.empty()) {
                std::string w = json.at(0).get<std::string>();
                makeUppercase(w);
                return w;
            }
            throw std::runtime_error("Unexpected JSON structure received from API");
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error(std::string("JSON parsing failed: ") + e.what());
        }
    }
    
    std::string Word = getWord();
    
    // correct location
    void isitCorrect(std::string g) {
        std::cout << " Hints :\n";
        // correct position
        for (int i=0; i<5; i++) {
            if (g[i] == Word[i]) {
                correct[i] = '2';
            }
        }
        // wrong position
        for (int i=0; i<5; i++) {
            if (g[i] == Word[i]) continue;
            for (int j=0; j<5; j++) {
                if (g[i] == Word[j] && i != j) correct[i] = '1';
            }
        }
    };

    void resetCorrect() {
        for (int i=0; i<5; i++) {
            correct[i] = 0;
        }
    }

    std::string hints(int n) {
        if (correct[n] == '1'){
            return " - Wrong Position.";
        } else if (correct[n] == '2') {
            return " - Correct Postion.";
        } else {
            return " - Not in word.";
        }
    };

public:
    void Start() {
        int numGuess=1;
        std::cout << Word << " : DEBUG\n";
        std::cout << "=== Welcome to Wordle! ===\n";
        while (true) {
            std::string guess;
            if (numGuess > 5) {std::cout << "You ran out of guesses! Better luck next time."; break;}
            std::cout << "Enter your "<< numGuess << suffix(numGuess) << " guess\n > "; std::cin >> guess;
            if (guess.length() == 5) {
                makeUppercase(guess);
                if (guess == Word) {
                    std::cout << "You guessed it the "<< numGuess << suffix(numGuess) << " time! The word was indeed " << Word << '!';
                    break;
                } else {
                    numGuess++;
                    resetCorrect(); isitCorrect(guess);
                    std::cout << "\033[H\033[2J" << " Hints :\n" << guess[0] << hints(0) << '\n'
                    << guess[1] << hints(1) << '\n'
                        << guess[2] << hints(2) << '\n'
                            << guess[3] << hints(3) << '\n'
                                << guess[4] << hints(4) << '\n';
                }

            } else {std::cout << "Please enter a 5-character word.\n";}
        }
        
    }
};

int main() {
    gameMain play;
    play.Start();
}
