#ifndef LEVELMENU_H
#define LEVELMENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class LevelMenu {
private:
    sf::Font font;
    std::vector<sf::Text> menuItems;
    int selectedIndex;
    std::vector<float> itemScales;
    bool fontLoaded;
    sf::RectangleShape background;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    float windowWidth;
    float windowHeight;

    // Snake animation variables
    std::vector<sf::RectangleShape> snakeSegments;
    float snakeSpeed;
    float snakeHeadX;

    bool tryLoadFont() {
        return font.loadFromFile("ARCADECLASSIC.ttf") ||
            font.loadFromFile("fonts/KnightWarrior.ttf") ||
            font.loadFromFile("assets/fonts/KnightWarrior.ttf") ||
            font.loadFromFile("Bruce Forever.ttf") ||
            font.loadFromFile("C:/Windows/Fonts/arial.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    }

public:
    LevelMenu(float width, float height) : selectedIndex(0), windowWidth(width), windowHeight(height), snakeSpeed(400.0f), snakeHeadX(0.0f) {
        fontLoaded = tryLoadFont();
        itemScales = { 1.0f, 1.0f, 1.0f };

        background.setSize(sf::Vector2f(width, height));
        background.setFillColor(sf::Color::Black);

        if (backgroundTexture.loadFromFile("Snake Menu.jpg")) {
            backgroundSprite.setTexture(backgroundTexture);
            sf::Vector2f targetSize(width, 570);
            backgroundSprite.setScale(
                targetSize.x / backgroundTexture.getSize().x,
                targetSize.y / backgroundTexture.getSize().y
            );
            backgroundSprite.setPosition(0, 0);
        }

        if (fontLoaded) {
            std::vector<std::string> itemNames = { "Level 1", "Level 2", "Level 3" };
            for (size_t i = 0; i < itemNames.size(); ++i) {
                sf::Text text;
                text.setFont(font);
                text.setCharacterSize(36);
                text.setString(itemNames[i]);
                text.setFillColor(sf::Color::White);
                text.setStyle(sf::Text::Bold);
                sf::FloatRect textBounds = text.getLocalBounds();
                text.setPosition((width - textBounds.width) / 2, 450 + i * 70);
                menuItems.push_back(text);
            }
            updateSelection();
        }

        // Initialize snake animation
        const int numSegments = 10;
        const float segmentSize = 20.0f;
        const float segmentSpacing = 25.0f;
        for (int i = 0; i < numSegments; ++i) {
            sf::RectangleShape segment(sf::Vector2f(segmentSize, segmentSize));
            segment.setFillColor(sf::Color::Green);
            segment.setPosition(snakeHeadX - i * segmentSpacing, height / 6.0f); // Moved higher
            snakeSegments.push_back(segment);
        }
    }

    void updateSelection() {
        for (size_t i = 0; i < menuItems.size(); ++i) {
            if (i == selectedIndex) {
                menuItems[i].setFillColor(sf::Color::Yellow);
                itemScales[i] = 1.2f;
            }
            else {
                menuItems[i].setFillColor(sf::Color::White);
                itemScales[i] = 1.0f;
            }
            menuItems[i].setScale(itemScales[i], itemScales[i]);
            sf::FloatRect textBounds = menuItems[i].getLocalBounds();
            menuItems[i].setPosition((windowWidth - textBounds.width * itemScales[i]) / 2, 450 + i * 70);
        }
    }

    void moveUp() {
        if (selectedIndex > 0) {
            selectedIndex--;
            updateSelection();
        }
    }

    void moveDown() {
        if (selectedIndex < static_cast<int>(menuItems.size()) - 1) {
            selectedIndex++;
            updateSelection();
        }
    }

    int getSelectedIndex() const {
        return selectedIndex;
    }

    bool handleMouseClick(float x, float y) {
        for (size_t i = 0; i < menuItems.size(); ++i) {
            if (menuItems[i].getGlobalBounds().contains(x, y)) {
                selectedIndex = static_cast<int>(i);
                updateSelection();
                return true;
            }
        }
        return false;
    }

    void handleMouseMove(float x, float y) {
        for (size_t i = 0; i < menuItems.size(); ++i) {
            if (menuItems[i].getGlobalBounds().contains(x, y) && selectedIndex != static_cast<int>(i)) {
                selectedIndex = static_cast<int>(i);
                updateSelection();
            }
        }
    }

    void update(float deltaTime) {
        for (size_t i = 0; i < menuItems.size(); ++i) {
            float targetScale = (i == selectedIndex) ? 1.2f : 1.0f;
            float currentScale = itemScales[i];
            float newScale = currentScale + (targetScale - currentScale) * deltaTime * 5.0f;
            itemScales[i] = newScale;
            menuItems[i].setScale(newScale, newScale);
            sf::FloatRect textBounds = menuItems[i].getLocalBounds();
            menuItems[i].setPosition((windowWidth - textBounds.width * newScale) / 2, 450 + i * 70);
        }

        // Update snake animation
        snakeHeadX += snakeSpeed * deltaTime;
        if (snakeHeadX > windowWidth + 25.0f * snakeSegments.size()) { // Account for full snake length
            snakeHeadX = -25.0f * snakeSegments.size(); // Reset to left side
        }
        for (size_t i = 0; i < snakeSegments.size(); ++i) {
            float segmentX = snakeHeadX - i * 25.0f;
            snakeSegments[i].setPosition(segmentX, windowHeight / 6.0f);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(background);
        if (backgroundTexture.getSize().x > 0) {
            window.draw(backgroundSprite);
        }
        if (fontLoaded) {
            for (const auto& item : menuItems) {
                window.draw(item);
            }
        }
        else {
            sf::Text errorText;
            errorText.setCharacterSize(24);
            errorText.setFillColor(sf::Color::Red);
            errorText.setString("Font loading failed!");
            errorText.setPosition(50, 50);
            window.draw(errorText);
        }

        // Draw snake segments last to ensure visibility
        for (const auto& segment : snakeSegments) {
            window.draw(segment);
        }
    }
};

#endif // LEVELMENU_H