#pragma once
#include <random>
#include <ctime>

class RandomEngine {
public:
    static std::mt19937 create() {
        try {
            std::random_device rd;
            std::seed_seq seeds{rd(), rd(), rd(), rd(), rd(), rd()};
            return std::mt19937(seeds);
        } catch (...) {
            return std::mt19937(static_cast<unsigned>(std::time(nullptr)));
        }
    }
};