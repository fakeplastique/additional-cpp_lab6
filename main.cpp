#include "random_generator.h"
#include "referee.h"
#include "player.h"
#include <iostream>
#include <memory>
#include <cmath>
#include <format>
#define DEMO 


const int FIRST_NUMBER = 1;
const int LAST_NUMBER = 100;

struct GuessResult {
    int number = std::numeric_limits<int>::min();
    int response = std::numeric_limits<int>::min();
};

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
            if (lastGuess->response == 1)
                end = std::min(end, lastGuess->number-1);
            else if (lastGuess->response == -1)
                start = std::max(start, lastGuess->number+1);
            else 
                break;
        }

        int guess = generator(start, end);
        int response = ref.guess(guess);

        lastGuess->number = guess;
        lastGuess->response = response;

        if (response == 1) {
            end = std::min(end, lastGuess->number-1);
        } else if (lastGuess->response == -1) {
            start = std::max(start, lastGuess->number+1); 
        } else {
            std::cout << std::format("Gambler player wins with: {} \n", guess);
            break;
        }
        std::cout << std::format("Gambler player chooses: {} \n", guess);

        co_yield guess;

        if (response == 0) break;
        
    }


}

Player binarySearchPlayer(
    Referee &ref, 
    std::shared_ptr<GuessResult> lastGuess) {
    int start = FIRST_NUMBER;
    int end = LAST_NUMBER;

    while (true) {

        if (lastGuess->response != std::numeric_limits<int>::min()) {
            if (lastGuess->response == 1)
                end = std::min(end, lastGuess->number-1);
            else if (lastGuess->response == -1)
                start = std::max(start, lastGuess->number+1);
            else 
                break;
        }


        int guess = (start + end) / 2;
        int response = ref.guess(guess);

        lastGuess->number = guess;
        lastGuess->response = response;

        
        if (response == 1) {
            end = std::min(end, lastGuess->number-1);
        } else if (lastGuess->response == -1) {
            start = std::max(start, lastGuess->number+1); 
        } else {
            std::cout << std::format("Binary search player wins with: {} \n", guess);
            break;
        }
        std::cout << std::format("Binary search player chooses: {} \n", guess);

        co_yield guess;

        
    }
}

int main() {

    RandomGenerator generator;

    std::shared_ptr<int> clientGuess = std::make_shared<int>(0);

    int numberToGuess = generator(FIRST_NUMBER, LAST_NUMBER);
    std::cout << "Number to guess: " << numberToGuess << std::endl;

    Referee ref = guessNumberGame(
        numberToGuess, 
        clientGuess
    );

    std::shared_ptr<GuessResult> lastGuessResult = std::make_shared<GuessResult>();
    Player player1 = gamblerPlayer(ref, lastGuessResult, generator);
    Player player2 = binarySearchPlayer(ref, lastGuessResult);

    std::vector<Player*> players{&player1, &player2}; 

    int turn = 0;
    while (lastGuessResult->response != 0) {
        Player* curr = players[turn % 2];
        if (curr->next_turn())
            curr->currentGuess();
        turn++;
    }

}