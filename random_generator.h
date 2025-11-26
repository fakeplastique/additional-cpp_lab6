#pragma once
#include "random_engine.h"

class RandomGenerator {
    private:
        mutable std::mt19937 engine_;

    public:
        RandomGenerator() : engine_(RandomEngine::create()) {}

        int operator()(int start, int end) const {
            std::uniform_int_distribution<int> dist(start, end);
            return dist(engine_);
        }
};