#ifndef LEVEL3_H
#define LEVEL3_H

#include <SFML/Graphics.hpp>

class Level3 {
public:
    static constexpr float moveInterval = 0.1f; // Same speed as Level 2

    // Color theme for Level 3
    static sf::Color getBackgroundColor() { return sf::Color(80, 20, 80); } // Dark purple background
    static sf::Color getCellColor1() { return sf::Color(150, 50, 150); } // Magenta cell
    static sf::Color getCellColor2() { return sf::Color(170, 70, 170); } // Lighter magenta cell
    static sf::Color getSnakeHeadColor() { return sf::Color(50, 200, 50); } // Green head
    static sf::Color getSnakeBodyColor() { return sf::Color(100, 255, 100); } // Light green body
    static sf::Color getAppleColor() { return sf::Color(255, 165, 0); } // Orange apple
    static sf::Color getBlueAppleColor() { return sf::Color(255, 105, 180); } // Pink blue apple
    static sf::Color getBombColor() { return sf::Color::Black; } // Black bomb
};

#endif // LEVEL3_H