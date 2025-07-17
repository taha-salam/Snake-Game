#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class PauseMenu {
private:
    std::vector<sf::Text> menuItems;
    sf::Text titleText;
    sf::Font font;
    int selectedIndex;
    float windowWidth;
    float windowHeight;
    std::vector<float> itemScales; // For animation
    sf::RectangleShape overlay;
    sf::RectangleShape menuContainer;
    sf::RectangleShape containerShadow;
    sf::VertexArray backgroundGradient;
    bool fontLoaded;
    int currentLevel; // To track current level for disabling "Next Level"

    bool tryLoadFont() {
        std::vector<std::string> fontPaths = {
            "ARCADECLASSIC.ttf",
            "fonts/KnightWarrior.ttf",
            "assets/fonts/KnightWarrior.ttf",
            "Bruce Forever.ttf",
            "impact.ttf",
            "bebas.ttf",
            "orbitron.ttf",
            "russo.ttf",
            "C:/Windows/Fonts/arialbd.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            "/System/Library/Fonts/Arial.ttf",
            "/Library/Fonts/Arial.ttf"
        };
        for (const auto& path : fontPaths) {
            if (font.loadFromFile(path)) return true;
        }
        return false;
    }

public:
    PauseMenu(float width, float height, int level) : selectedIndex(0), windowWidth(width), windowHeight(height), currentLevel(level) {
        fontLoaded = tryLoadFont();
        itemScales = { 1.0f, 1.0f, 1.0f };

        // Initialize gradient background
        backgroundGradient = sf::VertexArray(sf::Quads, 4);
        backgroundGradient[0].position = sf::Vector2f(0, 0);
        backgroundGradient[1].position = sf::Vector2f(width, 0);
        backgroundGradient[2].position = sf::Vector2f(width, height);
        backgroundGradient[3].position = sf::Vector2f(0, height);
        backgroundGradient[0].color = sf::Color(50, 50, 50);
        backgroundGradient[1].color = sf::Color(80, 80, 80);
        backgroundGradient[2].color = sf::Color(80, 80, 80);
        backgroundGradient[3].color = sf::Color(50, 50, 50);

        // Initialize overlay
        overlay.setSize(sf::Vector2f(width, height));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));

        // Initialize menu container shadow
        containerShadow.setSize(sf::Vector2f(400, 300));
        containerShadow.setPosition(width / 2 - 190, height / 2 - 120);
        containerShadow.setFillColor(sf::Color(0, 0, 0, 100));
        containerShadow.setOutlineColor(sf::Color::Transparent);

        // Initialize menu container
        menuContainer.setSize(sf::Vector2f(400, 300));
        menuContainer.setPosition(width / 2 - 200, height / 2 - 130);
        menuContainer.setFillColor(sf::Color(30, 30, 30, 240));
        menuContainer.setOutlineColor(sf::Color(255, 215, 0));
        menuContainer.setOutlineThickness(2);

        if (fontLoaded) {
            // Initialize title
            titleText.setFont(font);
            titleText.setString("Pause Menu");
            titleText.setCharacterSize(48);
            titleText.setFillColor(sf::Color(255, 215, 0));
            titleText.setStyle(sf::Text::Bold);
            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
            titleText.setPosition(width / 2, height / 2 - 180);

            // Initialize menu items
            std::vector<std::string> itemNames = { "Continue", "Next Level", "Exit" };
            for (size_t i = 0; i < itemNames.size(); ++i) {
                sf::Text text;
                text.setFont(font);
                text.setString(itemNames[i]);
                text.setCharacterSize(40);
                text.setStyle(sf::Text::Bold);
                if (i == 1 && currentLevel >= 3) {
                    text.setFillColor(sf::Color(100, 100, 100, 128)); // Dimmed for disabled
                }
                else {
                    text.setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
                }
                sf::FloatRect textBounds = text.getLocalBounds();
                text.setOrigin(textBounds.width / 2, textBounds.height / 2);
                text.setPosition(width / 2, height / 2 - 80 + i * 80);
                menuItems.push_back(text);
            }
        }
    }

    void moveUp() {
        if (selectedIndex > 0) {
            int newIndex = selectedIndex - 1;
            if (currentLevel >= 3 && newIndex == 1) {
                newIndex--;
            }
            if (newIndex >= 0) {
                menuItems[selectedIndex].setFillColor(sf::Color::White);
                selectedIndex = newIndex;
                menuItems[selectedIndex].setFillColor(sf::Color::Yellow);
            }
        }
    }

    void moveDown() {
        if (selectedIndex < static_cast<int>(menuItems.size()) - 1) {
            int newIndex = selectedIndex + 1;
            if (currentLevel >= 3 && newIndex == 1) {
                newIndex++;
            }
            if (newIndex < static_cast<int>(menuItems.size())) {
                menuItems[selectedIndex].setFillColor(sf::Color::White);
                selectedIndex = newIndex;
                menuItems[selectedIndex].setFillColor(sf::Color::Yellow);
            }
        }
    }

    bool handleMouseClick(float x, float y) {
        for (size_t i = 0; i < menuItems.size(); ++i) {
            if (menuItems[i].getGlobalBounds().contains(x, y)) {
                if (i == 1 && currentLevel >= 3) {
                    return false;
                }
                menuItems[selectedIndex].setFillColor(sf::Color::White);
                selectedIndex = i;
                menuItems[selectedIndex].setFillColor(sf::Color::Yellow);
                return true;
            }
        }
        return false;
    }

    void handleMouseMove(float x, float y) {
        for (size_t i = 0; i < menuItems.size(); ++i) {
            if (menuItems[i].getGlobalBounds().contains(x, y) && selectedIndex != static_cast<int>(i)) {
                if (i == 1 && currentLevel >= 3) {
                    continue;
                }
                menuItems[selectedIndex].setFillColor(sf::Color::White);
                selectedIndex = i;
                menuItems[selectedIndex].setFillColor(sf::Color::Yellow);
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
            menuItems[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
            menuItems[i].setPosition(windowWidth / 2, windowHeight / 2 - 80 + i * 80);
        }
    }

    void draw(sf::RenderWindow& window) {
        // Draw gradient background
        window.draw(backgroundGradient);
        // Draw overlay
        window.draw(overlay);
        // Draw menu container shadow and container
        window.draw(containerShadow);
        window.draw(menuContainer);
        // Draw title and menu items or error message
        if (fontLoaded) {
            window.draw(titleText);
            for (const auto& item : menuItems) {
                window.draw(item);
            }
        }
        else {
            sf::Text errorText;
            errorText.setFont(font);
            errorText.setCharacterSize(24);
            errorText.setFillColor(sf::Color::Red);
            errorText.setString("Font loading failed!");
            errorText.setPosition(50, 50);
            window.draw(errorText);
        }
    }

    int getSelectedIndex() const { return selectedIndex; }
};

#endif // PAUSEMENU_H