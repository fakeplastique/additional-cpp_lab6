#include <iostream>
#include <memory>
#include <format>
#include "random_generator.h"
#include "referee.h"
#include "player.h"


const int FIRST_NUMBER = 1;
const int LAST_NUMBER = 100;

struct GuessResult {
    int number = std::numeric_limits<int>::min();
    int response = std::numeric_limits<int>::min();
};


void updateRange(int& start, int& end, const int& guess, const int& response) {
    if (response == 1) {
        end = std::min(end, guess - 1);
    } else if (response == -1) {
        start = std::max(start, guess + 1);
    }
}


Referee guessNumberGame(int number, std::shared_ptr<int> clientGuess) {
    while (true) {
        int guess = *clientGuess;

        if (guess < number) {
            co_yield -1;
        } else if (guess > number) {
            co_yield 1;
        } else {
            co_yield 0;
            break;
        }

    }
}

Player gamblerPlayer(
    Referee &ref,
    std::shared_ptr<GuessResult> lastGuess,
    const RandomGenerator& generator) {
    
    int start = FIRST_NUMBER;
    int end = LAST_NUMBER;

    while (true) {

        if (lastGuess->response != std::numeric_limits<int>::min()) {
            if (lastGuess->response == 0) {
                break;
            }
            updateRange(start, end, lastGuess->number, lastGuess->response);
        }

        int guess = generator(start, end);
        int response = ref.guess(guess);

        lastGuess->number = guess;
        lastGuess->response = response;

        updateRange(start, end, guess, response);

        co_yield guess;
    }


}

Player binarySearchPlayer(
    Referee &ref, 
    std::shared_ptr<GuessResult> lastGuess) {
    int start = FIRST_NUMBER;
    int end = LAST_NUMBER;

    while (true) {

        if (lastGuess->response != std::numeric_limits<int>::min()) {
            if (lastGuess->response == 0) {
                break;
            }
            updateRange(start, end, lastGuess->number, lastGuess->response);
        }


        int guess = (start + end) / 2;
        int response = ref.guess(guess);

        lastGuess->number = guess;
        lastGuess->response = response;

        
        updateRange(start, end, guess, response);

        co_yield guess;
    }
}

int main() {

    RandomGenerator generator;

    std::shared_ptr<int> clientGuess = std::make_shared<int>(0);

    int numberToGuess = generator(FIRST_NUMBER, LAST_NUMBER);
    std::cout << "Number to guess: " << numberToGuess << std::endl;
    std::cout << "===================\n";

    Referee ref = guessNumberGame(
        numberToGuess, 
        clientGuess
    );

    std::shared_ptr<GuessResult> lastGuessResult = std::make_shared<GuessResult>();
    Player player1 = gamblerPlayer(ref, lastGuessResult, generator);
    Player player2 = binarySearchPlayer(ref, lastGuessResult);

    std::vector<Player*> players{&player1, &player2}; 
    std::vector<std::string> playerNames{"Gambler", "Binary Search"};

    int turn = 0;
    while (lastGuessResult->response != 0) {
        Player* player = players[turn % 2];
        if (player->next_turn()) {
            std::cout << std::format("{} player chooses: {}. ", playerNames[turn % 2], player->currentGuess());

            if (lastGuessResult->response) { 
                std::cout << ((lastGuessResult->response == -1) ? "Too small!" : "Too big!") << std::endl;
            } else {
                std::cout << std::endl;
                std::cout << std::format("{} player wins with {}!\n", playerNames[turn % 2], lastGuessResult->number);
            }
        }
        turn++;
    }


}