#pragma once

#include "Level2.h"
#include "Level3.h"

class ScoringSystem {
public:
    enum class Level { Level1, Level2, Level3 };

    ScoringSystem(Level level) : currentLevel(level) {}

    int getSmallAppleScore() const {
        switch (currentLevel) {
        case Level::Level1:
            return 1;
        case Level::Level2:
            return 2;
        case Level::Level3:
            return 3;
        default:
            return 0;
        }
    }

    int getBigAppleScore() const {
        switch (currentLevel) {
        case Level::Level1:
            return 2;
        case Level::Level2:
            return 4;
        case Level::Level3:
            return 6; // Assuming a logical progression (e.g., double the small apple score)
        default:
            return 0;
        }
    }

private:
    Level currentLevel;
};