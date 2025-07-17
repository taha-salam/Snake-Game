#ifndef LEVEL2_H
#define LEVEL2_H

#include <SFML/Graphics.hpp>

class Level2 {
public:
    static constexpr float moveInterval = 0.1f; // Faster snake speed (was 0.15f in Level 1)

    // Color theme for Level 2
    static sf::Color getBackgroundColor() { return sf::Color(50, 50, 100); } // Dark blue background
    static sf::Color getCellColor1() { return sf::Color(100, 100, 150); } // Light purple cell
    static sf::Color getCellColor2() { return sf::Color(120, 120, 170); } // Slightly lighter purple cell
    static sf::Color getSnakeHeadColor() { return sf::Color(200, 50, 50); } // Red head
    static sf::Color getSnakeBodyColor() { return sf::Color(255, 100, 100); } // Light red body
    static sf::Color getAppleColor() { return sf::Color(255, 255, 0); } // Yellow apple
    static sf::Color getBlueAppleColor() { return sf::Color(0, 255, 255); } // Cyan blue apple
    static sf::Color getBombColor() { return sf::Color::Black; } // Black bomb
};

#endif // LEVEL2_H