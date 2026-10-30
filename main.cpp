#include <cctype>
#include <iostream>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

struct Data {
    int i=1;
};

class Game {
    private:
        std::string count(Data& data) {
            if (data.i==1) {return "st";}
            else if (data.i==2) {return "nd";}
            else if (data.i==3) {return "rd";}
            else {return "th";}
        }

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

            std::string response;

            curl_easy_setopt(
                curl,
                CURLOPT_URL,
                "https://random-word-api.herokuapp.com/word?length=5"
            );

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponse);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

            const CURLcode result = curl_easy_perform(curl);

            curl_easy_cleanup(curl);

            if (result != CURLE_OK) {
                throw std::runtime_error(curl_easy_strerror(result));
            }

            // response looks like: ["house"]
            const nlohmann::json json =
                nlohmann::json::parse(response);

            return json.at(0).get<std::string>();
        }

        void Main(Data& data) {
            std::string Word = getWord();
            std::transform(Word.begin(), Word.end(), Word.begin(), [](unsigned char c){
                return toupper(c);
            });
            std::cout << "Welcome to Wordle!\n";
            std::string guess;

            while (true) {
                // if the user ran out of guesses
                if (data.i > 6) {
                    std::cout << "You ran out of guesses.\nThe word was " << Word << '.';
                    break;
                }
                
                std::cout << data.i << count(data) << " guess. Enter your guess\n > "; std::cin >> guess;

                if (guess.length() == 5) {
                    // transform to upper
                    std::transform(guess.begin(), guess.end(), guess.begin(), [](unsigned char c){
                        return toupper(c);
                    });

                    // IF CORRECT
                    if (guess == Word) {std::cout << "You guessed it the " << data.i << count(data) << " time! The word was indeed " << Word << '.'; break;}
                    
                    // IF SOMETHING IS INCORRECT
                    else {
                        data.i++;

                        // correct position
                        for (int i=0; i<5; i++) {
                            if (guess[i] == Word[i]) {
                                std::cout << "The letter '" << guess[i] << "' is in the correct position\n";
                            }
                        }

                        // wrong location
                        for (int i = 0; i < 5; i++) {
                            // Skip this letter if we already established it is in the correct position
                            if (guess[i] == Word[i]) continue; 
                            
                            // Check if the current guessed letter exists anywhere else in the Word
                            for (int j = 0; j < 5; j++) {
                                if (guess[i] == Word[j] && i != j) {
                                    std::cout << "The letter '" << guess[i] << "' is in the wrong place.\n";
                                    break; // Found a match, break out of the inner loop to check the next letter
                                }
                            }
                        }
                    }
                } else {std::cout << "Word needs to be 5 characters long.\n";}
            }
                
        }
    public:
        void Start(Data& data) {
            Main(data);
        }

};

int main() {
    Data data; Game menu;
    menu.Start(data);

    return 0;
}