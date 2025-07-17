#include "SFML/Graphics.hpp"
#include "Menu.h"
#include "PauseMenu.h"
#include "LevelMenu.h"
#include "Level2.h"
#include "Level3.h"
#include "ScoringSystem.h"
#include <vector>
#include <random>
#include <algorithm>

struct Position {
    int x, y;
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class Snake {
private:
    std::vector<Position> body;
    Position direction;
    bool growing;

public:
    Snake() : direction{ 1, 0 }, growing(false) {
        body.push_back({ 5, 9 });
        body.push_back({ 4, 9 });
        body.push_back({ 3, 9 });
    }

    void setDirection(int dx, int dy) {
        if (body.size() > 1 && direction.x == -dx && direction.y == -dy) return;
        direction = { dx, dy };
    }

    void update() {
        Position newHead = body[0];
        newHead.x += direction.x;
        newHead.y += direction.y;
        body.insert(body.begin(), newHead);

        if (!growing) body.pop_back();
        else growing = false;
    }

    void grow() { growing = true; }

    Position getHead() const { return body[0]; }

    std::vector<Position>& getBody() { return body; }
    const std::vector<Position>& getBody() const { return body; }

    bool checkSelfCollision() const {
        const Position& head = body[0];
        for (size_t i = 1; i < body.size(); ++i) {
            if (body[i] == head) return true;
        }
        return false;
    }

    bool checkWallCollision(int cols, int rows) const {
        const Position& head = body[0];
        return head.x < 0 || head.x >= cols || head.y < 0 || head.y >= rows;
    }
};

class Apple {
private:
    Position position;
    std::mt19937 rng;
    std::uniform_int_distribution<int> distX, distY;

public:
    Apple(int cols, int rows)
        : rng(std::random_device{}()), distX(0, cols - 1), distY(0, rows - 1) {
        respawn(cols, rows, {});
    }

    void respawn(int cols, int rows, const std::vector<Position>& snakeBody) {
        do {
            position.x = distX(rng);
            position.y = distY(rng);
        } while (std::find(snakeBody.begin(), snakeBody.end(), position) != snakeBody.end() ||
            position.x < 0 || position.x >= cols || position.y < 0 || position.y >= rows);
    }

    void setDistribution(int cols, int rows) {
        distX = std::uniform_int_distribution<int>(0, cols - 1);
        distY = std::uniform_int_distribution<int>(0, rows - 1);
    }

    Position getPosition() const { return position; }
};

class Bomb {
private:
    Position position;
    std::mt19937 rng;
    std::uniform_int_distribution<int> distX, distY;

public:
    Bomb(int cols, int rows)
        : rng(std::random_device{}()), distX(0, cols - 1), distY(0, rows - 1) {
        respawn(cols, rows, {});
    }

    void respawn(int cols, int rows, const std::vector<Position>& snakeBody) {
        do {
            position.x = distX(rng);
            position.y = distY(rng);
        } while (std::find(snakeBody.begin(), snakeBody.end(), position) != snakeBody.end() ||
            position.x < 0 || position.x >= cols || position.y < 0 || position.y >= rows);
    }

    void setDistribution(int cols, int rows) {
        distX = std::uniform_int_distribution<int>(0, cols - 1);
        distY = std::uniform_int_distribution<int>(0, rows - 1);
    }

    Position getPosition() const { return position; }
};

sf::Font loadBestFont() {
    sf::Font font;
    std::vector<std::string> fontPaths = {
        "impact.ttf", // Prioritize ARCADECLASSIC.ttf
        "bebas.ttf", "orbitron.ttf", "russo.ttf",
        "arialbd.ttf", "arial.ttf",
        "C:/Windows/Fonts/impact.ttf", "C:/Windows/Fonts/arialbd.ttf", "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/System/Library/Fonts/Arial.ttf", "/Library/Fonts/Arial.ttf"
    };
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) break;
    }
    return font;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1300, 800), "Snake Game");
    const int cellSize = 40;
    int rows = 18;
    int cols = 23;

    enum class GameState { Menu, LevelSelect, Playing, Paused, GameOver, About };
    enum class Level { Level1, Level2, Level3 };
    GameState gameState = GameState::Menu;
    Level currentLevel = Level::Level1;
    Menu menu(1300, 800);
    LevelMenu levelMenu(1300, 800);
    int pauseLevel = (currentLevel == Level::Level1) ? 1 : (currentLevel == Level::Level2) ? 2 : 3;
    PauseMenu pauseMenu(1300, 800, pauseLevel);
    ScoringSystem scoringSystem(ScoringSystem::Level::Level1);

    Snake snake;
    Apple apple(cols, rows);
    Apple blueApple(cols, rows);
    Bomb bomb(cols, rows);
    bool blueAppleVisible = false;
    bool bombVisible = false;
    float blueAppleTimer = 0.0f;
    float blueAppleVisibleTimer = 0.0f;
    float bombTimer = 0.0f;
    float bombVisibleTimer = 0.0f;
    float wallShrinkTimer = 0.0f;
    const float blueAppleInterval = 10.0f;
    const float blueAppleVisibleDuration = 3.0f;
    const float bombInterval = 8.0f;
    const float bombVisibleDuration = 3.0f;
    const float wallShrinkInterval = 5.0f;

    sf::Clock clock;
    float moveInterval = 0.15f;
    float timeSinceLastMove = 0.0f;
    int score = 0;
    int appleCount = 0;
    bool gameOver = false;

    sf::Font font = loadBestFont();
    sf::Text scoreText, applesText, gameOverText, restartText, helpTitle, helpText, backButtonText, instructionsText;
    bool fontLoaded = !font.getInfo().family.empty();

    if (fontLoaded) {
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color(255, 215, 0));
        scoreText.setStyle(sf::Text::Bold);

        applesText.setFont(font);
        applesText.setCharacterSize(24);
        applesText.setFillColor(sf::Color(255, 215, 0));
        applesText.setStyle(sf::Text::Bold);

        gameOverText.setFont(font);
        gameOverText.setCharacterSize(64);
        gameOverText.setFillColor(sf::Color(255, 50, 50));
        gameOverText.setStyle(sf::Text::Bold);
        gameOverText.setString("GAME OVER!");
        sf::FloatRect textBounds = gameOverText.getLocalBounds();
        gameOverText.setPosition((960 - textBounds.width) / 2, 800 / 2 - 80);

        restartText.setFont(font);
        restartText.setCharacterSize(26);
        restartText.setFillColor(sf::Color::Yellow);
        restartText.setStyle(sf::Text::Bold);
        restartText.setString("Press R to Restart or ESC for Menu");
        sf::FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setPosition((960 - restartBounds.width) / 2, 800 / 2 - 20);

        helpTitle.setFont(font);
        helpTitle.setCharacterSize(40);
        helpTitle.setFillColor(sf::Color(255, 215, 0));
        helpTitle.setStyle(sf::Text::Bold);
        helpTitle.setString("Help - Snake Game");

        helpText.setFont(font);
        helpText.setCharacterSize(16);
        helpText.setFillColor(sf::Color::White);
        helpText.setString("Snake Game - Use arrow keys to move\n"
                          "Eat apples to grow and increase your score\n"
                          "Avoid hitting walls, yourself, or bombs!\n"
                          "Walls shrink every 5 seconds in Level 3!\n\n"
                          "Scoring:\n"
                          "- Level 1: Red Apple +1, Big Blue Apple +2\n"
                          "- Level 2: Red Apple +2, Big Blue Apple +4\n"
                          "- Level 3: Red Apple +3, Big Blue Apple +6\n\n"
                          "Controls:\n"
                          "  Arrow Keys: Move\n"
                          "  P: Pause\n"
                          "  R: Restart\n"
                          "  ESC: Menu");

        backButtonText.setFont(font);
        backButtonText.setCharacterSize(24);
        backButtonText.setFillColor(sf::Color::White);
        backButtonText.setStyle(sf::Text::Bold);
        backButtonText.setString("Back");

        instructionsText.setFont(font);
        instructionsText.setCharacterSize(20);
        instructionsText.setFillColor(sf::Color(255, 215, 0));
        instructionsText.setStyle(sf::Text::Bold);
        instructionsText.setLineSpacing(1.2f);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (gameState == GameState::Menu) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) menu.moveUp();
                    else if (event.key.code == sf::Keyboard::Down) menu.moveDown();
                    else if (event.key.code == sf::Keyboard::Enter) {
                        int selection = menu.getSelectedIndex();
                        if (selection == 0) {
                            gameState = GameState::LevelSelect;
                        }
                        else if (selection == 1 || selection == 2) {
                            gameState = GameState::About;
                        }
                        else if (selection == 3) {
                            window.close();
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (menu.handleMouseClick(event.mouseButton.x, event.mouseButton.y)) {
                        int selection = menu.getSelectedIndex();
                        if (selection == 0) {
                            gameState = GameState::LevelSelect;
                        }
                        else if (selection == 1 || selection == 2) {
                            gameState = GameState::About;
                        }
                        else if (selection == 3) {
                            window.close();
                        }
                    }
                }
                else if (event.type == sf::Event::MouseMoved) {
                    menu.handleMouseMove(event.mouseMove.x, event.mouseMove.y);
                }
            }
            else if (gameState == GameState::LevelSelect) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) levelMenu.moveUp();
                    else if (event.key.code == sf::Keyboard::Down) levelMenu.moveDown();
                    else if (event.key.code == sf::Keyboard::Enter) {
                        int selection = levelMenu.getSelectedIndex();
                        gameState = GameState::Playing;
                        if (selection == 0) {
                            currentLevel = Level::Level1;
                            moveInterval = 0.15f;
                            cols = 23;
                            rows = 18;
                            scoringSystem = ScoringSystem(ScoringSystem::Level::Level1);
                        }
                        else if (selection == 1) {
                            currentLevel = Level::Level2;
                            moveInterval = Level2::moveInterval;
                            cols = 23;
                            rows = 18;
                            scoringSystem = ScoringSystem(ScoringSystem::Level::Level2);
                        }
                        else if (selection == 2) {
                            currentLevel = Level::Level3;
                            moveInterval = Level3::moveInterval;
                            cols = 23;
                            rows = 18;
                            scoringSystem = ScoringSystem(ScoringSystem::Level::Level3);
                        }
                        pauseLevel = (currentLevel == Level::Level1) ? 1 : (currentLevel == Level::Level2) ? 2 : 3;
                        pauseMenu = PauseMenu(1300, 800, pauseLevel);
                        snake = Snake();
                        apple.setDistribution(cols, rows);
                        blueApple.setDistribution(cols, rows);
                        bomb.setDistribution(cols, rows);
                        apple.respawn(cols, rows, snake.getBody());
                        blueApple.respawn(cols, rows, snake.getBody());
                        bomb.respawn(cols, rows, snake.getBody());
                        score = 0;
                        appleCount = 0;
                        gameOver = false;
                        blueAppleVisible = false;
                        blueAppleTimer = 0.0f;
                        blueAppleVisibleTimer = 0.0f;
                        bombVisible = false;
                        bombTimer = 0.0f;
                        bombVisibleTimer = 0.0f;
                        wallShrinkTimer = 0.0f;
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        gameState = GameState::Menu;
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (levelMenu.handleMouseClick(event.mouseButton.x, event.mouseButton.y)) {
                        int selection = levelMenu.getSelectedIndex();
                        gameState = GameState::Playing;
                        if (selection == 0) {
                            currentLevel = Level::Level1;
                            moveInterval = 0.15f;
                            cols = 23;
                            rows = 18;
                            scoringSystem = ScoringSystem(ScoringSystem::Level::Level1);
                        }
                        else if (selection == 1) {
                            currentLevel = Level::Level2;
                            moveInterval = Level2::moveInterval;
                            cols = 23;
                            rows = 18;
                            scoringSystem = ScoringSystem(ScoringSystem::Level::Level2);
                        }
                        else if (selection == 2) {
                            currentLevel = Level::Level3;
                            moveInterval = Level3::moveInterval;
                            cols = 23;
                            rows = 18;
                            scoringSystem = ScoringSystem(ScoringSystem::Level::Level3);
                        }
                        pauseLevel = (currentLevel == Level::Level1) ? 1 : (currentLevel == Level::Level2) ? 2 : 3;
                        pauseMenu = PauseMenu(1300, 800, pauseLevel);
                        snake = Snake();
                        apple.setDistribution(cols, rows);
                        blueApple.setDistribution(cols, rows);
                        bomb.setDistribution(cols, rows);
                        apple.respawn(cols, rows, snake.getBody());
                        blueApple.respawn(cols, rows, snake.getBody());
                        bomb.respawn(cols, rows, snake.getBody());
                        score = 0;
                        appleCount = 0;
                        gameOver = false;
                        blueAppleVisible = false;
                        blueAppleTimer = 0.0f;
                        blueAppleVisibleTimer = 0.0f;
                        bombVisible = false;
                        bombTimer = 0.0f;
                        bombVisibleTimer = 0.0f;
                        wallShrinkTimer = 0.0f;
                    }
                }
                else if (event.type == sf::Event::MouseMoved) {
                    levelMenu.handleMouseMove(event.mouseMove.x, event.mouseMove.y);
                }
            }
            else if (gameState == GameState::Playing) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::P) {
                        gameState = GameState::Paused;
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        gameState = GameState::Menu;
                    }
                    else {
                        switch (event.key.code) {
                        case sf::Keyboard::Up: snake.setDirection(0, -1); break;
                        case sf::Keyboard::Down: snake.setDirection(0, 1); break;
                        case sf::Keyboard::Left: snake.setDirection(-1, 0); break;
                        case sf::Keyboard::Right: snake.setDirection(1, 0); break;
                        }
                    }
                }
            }
            else if (gameState == GameState::GameOver) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::R) {
                        gameState = GameState::Playing;
                        cols = 23;
                        rows = 18;
                        snake = Snake();
                        apple.setDistribution(cols, rows);
                        blueApple.setDistribution(cols, rows);
                        bomb.setDistribution(cols, rows);
                        apple.respawn(cols, rows, snake.getBody());
                        blueApple.respawn(cols, rows, snake.getBody());
                        bomb.respawn(cols, rows, snake.getBody());
                        score = 0;
                        appleCount = 0;
                        gameOver = false;
                        blueAppleVisible = false;
                        blueAppleTimer = 0.0f;
                        blueAppleVisibleTimer = 0.0f;
                        bombVisible = false;
                        bombTimer = 0.0f;
                        bombVisibleTimer = 0.0f;
                        wallShrinkTimer = 0.0f;
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        gameState = GameState::Menu;
                    }
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        if (gameState == GameState::Menu) {
            menu.update(deltaTime);
        }
        else if (gameState == GameState::LevelSelect) {
            levelMenu.update(deltaTime);
        }
        else if (gameState == GameState::Playing && !gameOver) {
            timeSinceLastMove += deltaTime;
            blueAppleTimer += deltaTime;
            if (currentLevel == Level::Level2 || currentLevel == Level::Level3) {
                bombTimer += deltaTime;
            }
            if (currentLevel == Level::Level3) {
                wallShrinkTimer += deltaTime;
            }
            if (blueAppleVisible) {
                blueAppleVisibleTimer += deltaTime;
                if (blueAppleVisibleTimer >= blueAppleVisibleDuration) {
                    blueAppleVisible = false;
                    blueAppleVisibleTimer = 0.0f;
                }
            }
            if (bombVisible && (currentLevel == Level::Level2 || currentLevel == Level::Level3)) {
                bombVisibleTimer += deltaTime;
                if (bombVisibleTimer >= bombVisibleDuration) {
                    bombVisible = false;
                    bombVisibleTimer = 0.0f;
                }
            }

            if (currentLevel == Level::Level3 && wallShrinkTimer >= wallShrinkInterval) {
                if (cols > 5 && rows > 5) {
                    cols--;
                    rows--;
                    apple.setDistribution(cols, rows);
                    blueApple.setDistribution(cols, rows);
                    bomb.setDistribution(cols, rows);
                    auto& snakeBody = snake.getBody();
                    for (auto& segment : snakeBody) {
                        if (segment.x >= cols) segment.x = cols - 1;
                        if (segment.y >= rows) segment.y = rows - 1;
                    }
                    if (apple.getPosition().x >= cols || apple.getPosition().y >= rows) {
                        apple.respawn(cols, rows, snakeBody);
                    }
                    if (blueApple.getPosition().x >= cols || blueApple.getPosition().y >= rows) {
                        blueApple.respawn(cols, rows, snakeBody);
                    }
                    if (bomb.getPosition().x >= cols || bomb.getPosition().y >= rows) {
                        bomb.respawn(cols, rows, snakeBody);
                    }
                    wallShrinkTimer = 0.0f;
                }
            }

            if (timeSinceLastMove >= moveInterval) {
                timeSinceLastMove = 0.0f;
                snake.update();

                if (snake.checkWallCollision(cols, rows) || snake.checkSelfCollision()) {
                    gameOver = true;
                    gameState = GameState::GameOver;
                }

                if (snake.getHead() == apple.getPosition()) {
                    snake.grow();
                    score += scoringSystem.getSmallAppleScore();
                    appleCount += 1;
                    apple.respawn(cols, rows, snake.getBody());
                }

                if (blueAppleVisible && snake.getHead() == blueApple.getPosition()) {
                    snake.grow();
                    snake.grow();
                    score += scoringSystem.getBigAppleScore();
                    appleCount += 1;
                    blueAppleVisible = false;
                    blueAppleVisibleTimer = 0.0f;
                }

                if (bombVisible && (currentLevel == Level::Level2 || currentLevel == Level::Level3) && snake.getHead() == bomb.getPosition()) {
                    gameOver = true;
                    gameState = GameState::GameOver;
                }
            }

            if (!blueAppleVisible && blueAppleTimer >= blueAppleInterval) {
                blueApple.respawn(cols, rows, snake.getBody());
                blueAppleVisible = true;
                blueAppleVisibleTimer = 0.0f;
                blueAppleTimer = 0.0f;
            }

            if (!bombVisible && (currentLevel == Level::Level2 || currentLevel == Level::Level3) && bombTimer >= bombInterval) {
                bomb.respawn(cols, rows, snake.getBody());
                bombVisible = true;
                bombVisibleTimer = 0.0f;
                bombTimer = 0.0f;
            }
        }
        else if (gameState == GameState::Paused) {
            sf::RenderWindow pauseWindow(sf::VideoMode(800, 600), "Pause Menu", sf::Style::Titlebar | sf::Style::Close);
            pauseWindow.setPosition(sf::Vector2i(window.getPosition().x + 250, window.getPosition().y + 100));
            PauseMenu pauseMenuInstance(800, 600, pauseLevel);

            while (pauseWindow.isOpen()) {
                sf::Event pauseEvent;
                while (pauseWindow.pollEvent(pauseEvent)) {
                    if (pauseEvent.type == sf::Event::Closed || pauseEvent.key.code == sf::Keyboard::Escape) {
                        pauseWindow.close();
                        gameState = GameState::Playing;
                    }
                    if (pauseEvent.type == sf::Event::KeyPressed) {
                        if (pauseEvent.key.code == sf::Keyboard::Up) {
                            pauseMenuInstance.moveUp();
                        }
                        else if (pauseEvent.key.code == sf::Keyboard::Down) {
                            pauseMenuInstance.moveDown();
                        }
                        else if (pauseEvent.key.code == sf::Keyboard::Enter) {
                            int selection = pauseMenuInstance.getSelectedIndex();
                            if (selection == 0) {
                                pauseWindow.close();
                                gameState = GameState::Playing;
                            }
                            else if (selection == 1 && pauseLevel < 3) {
                                if (pauseLevel == 1) {
                                    currentLevel = Level::Level2;
                                    moveInterval = Level2::moveInterval;
                                    scoringSystem = ScoringSystem(ScoringSystem::Level::Level2);
                                }
                                else if (pauseLevel == 2) {
                                    currentLevel = Level::Level3;
                                    moveInterval = Level3::moveInterval;
                                    scoringSystem = ScoringSystem(ScoringSystem::Level::Level3);
                                }
                                pauseLevel = (currentLevel == Level::Level1) ? 1 : (currentLevel == Level::Level2) ? 2 : 3;
                                pauseMenu = PauseMenu(1300, 800, pauseLevel);
                                cols = 23;
                                rows = 18;
                                snake = Snake();
                                apple.setDistribution(cols, rows);
                                blueApple.setDistribution(cols, rows);
                                bomb.setDistribution(cols, rows);
                                apple.respawn(cols, rows, snake.getBody());
                                blueApple.respawn(cols, rows, snake.getBody());
                                bomb.respawn(cols, rows, snake.getBody());
                                score = 0;
                                appleCount = 0;
                                gameOver = false;
                                blueAppleVisible = false;
                                blueAppleTimer = 0.0f;
                                blueAppleVisibleTimer = 0.0f;
                                bombVisible = false;
                                bombTimer = 0.0f;
                                bombVisibleTimer = 0.0f;
                                wallShrinkTimer = 0.0f;
                                pauseWindow.close();
                                gameState = GameState::Playing;
                            }
                            else if (selection == 2) {
                                pauseWindow.close();
                                gameState = GameState::Menu;
                            }
                        }
                    }
                    else if (pauseEvent.type == sf::Event::MouseButtonPressed && pauseEvent.mouseButton.button == sf::Mouse::Left) {
                        if (pauseMenuInstance.handleMouseClick(pauseEvent.mouseButton.x, pauseEvent.mouseButton.y)) {
                            int selection = pauseMenuInstance.getSelectedIndex();
                            if (selection == 0) {
                                pauseWindow.close();
                                gameState = GameState::Playing;
                            }
                            else if (selection == 1 && pauseLevel < 3) {
                                if (pauseLevel == 1) {
                                    currentLevel = Level::Level2;
                                    moveInterval = Level2::moveInterval;
                                    scoringSystem = ScoringSystem(ScoringSystem::Level::Level2);
                                }
                                else if (pauseLevel == 2) {
                                    currentLevel = Level::Level3;
                                    moveInterval = Level3::moveInterval;
                                    scoringSystem = ScoringSystem(ScoringSystem::Level::Level3);
                                }
                                pauseLevel = (currentLevel == Level::Level1) ? 1 : (currentLevel == Level::Level2) ? 2 : 3;
                                pauseMenu = PauseMenu(1300, 800, pauseLevel);
                                cols = 23;
                                rows = 18;
                                snake = Snake();
                                apple.setDistribution(cols, rows);
                                blueApple.setDistribution(cols, rows);
                                bomb.setDistribution(cols, rows);
                                apple.respawn(cols, rows, snake.getBody());
                                blueApple.respawn(cols, rows, snake.getBody());
                                bomb.respawn(cols, rows, snake.getBody());
                                score = 0;
                                appleCount = 0;
                                gameOver = false;
                                blueAppleVisible = false;
                                blueAppleTimer = 0.0f;
                                blueAppleVisibleTimer = 0.0f;
                                bombVisible = false;
                                bombTimer = 0.0f;
                                bombVisibleTimer = 0.0f;
                                wallShrinkTimer = 0.0f;
                                pauseWindow.close();
                                gameState = GameState::Playing;
                            }
                            else if (selection == 2) {
                                pauseWindow.close();
                                gameState = GameState::Menu;
                            }
                        }
                    }
                    else if (pauseEvent.type == sf::Event::MouseMoved) {
                        pauseMenuInstance.handleMouseMove(pauseEvent.mouseMove.x, pauseEvent.mouseMove.y);
                    }
                }

                pauseMenuInstance.update(deltaTime);
                pauseWindow.clear(sf::Color::Black);
                pauseMenuInstance.draw(pauseWindow);
                pauseWindow.display();
            }
        }

        window.clear(currentLevel == Level::Level2 ? Level2::getBackgroundColor() :
            currentLevel == Level::Level3 ? Level3::getBackgroundColor() :
            (gameState == GameState::Menu || gameState == GameState::LevelSelect ? sf::Color::Black : sf::Color(34, 139, 34)));

        if (gameState == GameState::Menu) {
            menu.draw(window);
        }
        else if (gameState == GameState::LevelSelect) {
            levelMenu.draw(window);
        }
        else if (gameState == GameState::Playing || gameState == GameState::GameOver) {
            sf::RectangleShape instructionPanel(sf::Vector2f(300, 720));
            instructionPanel.setPosition(980, 40);
            sf::Vertex gradient[] = {
                sf::Vertex(sf::Vector2f(980, 40), sf::Color(20, 80, 20)),
                sf::Vertex(sf::Vector2f(1280, 40), sf::Color(30, 90, 30)),
                sf::Vertex(sf::Vector2f(980, 760), sf::Color(20, 80, 20)),
                sf::Vertex(sf::Vector2f(1280, 760), sf::Color(30, 90, 30))
            };
            window.draw(gradient, 4, sf::Quads);
            instructionPanel.setOutlineColor(sf::Color(139, 69, 19));
            instructionPanel.setOutlineThickness(2);
            instructionPanel.setFillColor(sf::Color::Transparent);
            window.draw(instructionPanel);

            for (int i = 0; i < cols; ++i) {
                for (int j = 0; j < rows; ++j) {
                    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                    cell.setPosition(40 + i * cellSize, 40 + j * cellSize);
                    if (currentLevel == Level::Level2) {
                        cell.setFillColor((i + j) % 2 == 0 ? Level2::getCellColor1() : Level2::getCellColor2());
                    }
                    else if (currentLevel == Level::Level3) {
                        cell.setFillColor((i + j) % 2 == 0 ? Level3::getCellColor1() : Level3::getCellColor2());
                    }
                    else {
                        cell.setFillColor((i + j) % 2 == 0 ? sf::Color(144, 238, 144) : sf::Color(152, 251, 152));
                    }
                    window.draw(cell);
                }
            }

            if (currentLevel == Level::Level3) {
                sf::RectangleShape wall(sf::Vector2f(1300, 800));
                wall.setFillColor(sf::Color(0, 0, 0, 128));
                wall.setPosition(0, 0);
                sf::RectangleShape playableArea(sf::Vector2f(cols * cellSize, rows * cellSize));
                playableArea.setPosition(40, 40);
                wall.setTextureRect(sf::IntRect(0, 0, 1300, 800));
                playableArea.setFillColor(sf::Color::Transparent);
                window.draw(wall, sf::BlendAlpha);
                window.setView(sf::View(sf::FloatRect(0, 0, 1300, 800)));
            }

            const auto& snakeBody = snake.getBody();
            for (size_t i = 0; i < snakeBody.size(); ++i) {
                sf::RectangleShape segment(sf::Vector2f(cellSize - 2, cellSize - 2));
                segment.setPosition(40 + snakeBody[i].x * cellSize + 1, 40 + snakeBody[i].y * cellSize + 1);
                if (currentLevel == Level::Level2) {
                    segment.setFillColor(i == 0 ? Level2::getSnakeHeadColor() : Level2::getSnakeBodyColor());
                }
                else if (currentLevel == Level::Level3) {
                    segment.setFillColor(i == 0 ? Level3::getSnakeHeadColor() : Level3::getSnakeBodyColor());
                }
                else {
                    segment.setFillColor(i == 0 ? sf::Color(0, 0, 139) : sf::Color(65, 105, 225));
                }
                window.draw(segment);
            }

            sf::CircleShape appleShape(cellSize / 2 - 2);
            appleShape.setPosition(40 + apple.getPosition().x * cellSize + 2, 40 + apple.getPosition().y * cellSize + 2);
            appleShape.setFillColor(currentLevel == Level::Level2 ? Level2::getAppleColor() :
                currentLevel == Level::Level3 ? Level3::getAppleColor() : sf::Color::Red);
            window.draw(appleShape);

            if (blueAppleVisible) {
                sf::CircleShape blueShape(cellSize / 2 + 2);
                blueShape.setPosition(40 + blueApple.getPosition().x * cellSize - 2, 40 + blueApple.getPosition().y * cellSize - 2);
                blueShape.setFillColor(currentLevel == Level::Level2 ? Level2::getBlueAppleColor() :
                    currentLevel == Level::Level3 ? Level3::getBlueAppleColor() : sf::Color::Blue);
                window.draw(blueShape);
            }

            if (bombVisible && (currentLevel == Level::Level2 || currentLevel == Level::Level3)) {
                sf::CircleShape bombShape(cellSize / 2);
                bombShape.setPosition(40 + bomb.getPosition().x * cellSize, 40 + bomb.getPosition().y * cellSize);
                bombShape.setFillColor(currentLevel == Level::Level2 ? Level2::getBombColor() :
                    currentLevel == Level::Level3 ? Level3::getBombColor() : sf::Color::Black);
                window.draw(bombShape);
            }

            if (fontLoaded) {
                std::string instructions;
                if (currentLevel == Level::Level1) {
                    instructions = "Instructions:\n"
                                  "- Red Apple: +1 point,\n  grows snake by 1\n"
                                  "- Big Blue Apple: +2 points,\n  grows snake by 2\n"
                                  "- Controls:\n"
                                  "  Arrow Keys: Move\n"
                                  "  P: Pause\n"
                                  "  R: Restart\n"
                                  "  ESC: Menu";
                }
                else if (currentLevel == Level::Level2) {
                    instructions = "Instructions:\n"
                                  "- Red Apple: +2 points,\n  grows snake by 1\n"
                                  "- Big Blue Apple: +4 points,\n  grows snake by 2\n"
                                  "- Black Bomb: Game over\n"
                                  "- Controls:\n"
                                  "  Arrow Keys: Move\n"
                                  "  P: Pause\n"
                                  "  R: Restart\n"
                                  "  ESC: Menu";
                }
                else if (currentLevel == Level::Level3) {
                    instructions = "Instructions:\n"
                                  "- Red Apple: +3 points,\n  grows snake by 1\n"
                                  "- Big Blue Apple: +6 points,\n  grows snake by 2\n"
                                  "- Black Bomb: Game over\n"
                                  "- Walls shrink every 5s\n"
                                  "- Controls:\n"
                                  "  Arrow Keys: Move\n"
                                  "  P: Pause\n"
                                  "  R: Restart\n"
                                  "  ESC: Menu";
                }
                instructionsText.setString(instructions);
                instructionsText.setPosition(990, 50);
                window.draw(instructionsText);

                sf::RectangleShape headerPanel(sf::Vector2f(960, 30));
                headerPanel.setPosition(40, 5);
                sf::Vertex headerGradient[] = {
                    sf::Vertex(sf::Vector2f(40, 5), sf::Color(20, 80, 20)),
                    sf::Vertex(sf::Vector2f(1000, 5), sf::Color(30, 90, 30)),
                    sf::Vertex(sf::Vector2f(40, 35), sf::Color(20, 80, 20)),
                    sf::Vertex(sf::Vector2f(1000, 35), sf::Color(30, 90, 30))
                };
                window.draw(headerGradient, 4, sf::Quads);
                headerPanel.setOutlineColor(sf::Color(139, 69, 19));
                headerPanel.setOutlineThickness(1);
                headerPanel.setFillColor(sf::Color::Transparent);
                window.draw(headerPanel);

                scoreText.setString("Score: " + std::to_string(score));
                sf::FloatRect scoreBounds = scoreText.getLocalBounds();
                scoreText.setPosition(50, 8);
                window.draw(scoreText);

                sf::CircleShape appleIcon(10);
                appleIcon.setFillColor(currentLevel == Level::Level2 ? Level2::getAppleColor() :
                                       currentLevel == Level::Level3 ? Level3::getAppleColor() : sf::Color::Red);
                appleIcon.setPosition(960 - 120, 10);
                window.draw(appleIcon);

                applesText.setString(": " + std::to_string(appleCount));
                applesText.setPosition(960 - 100, 8);
                window.draw(applesText);

                if (gameState == GameState::GameOver) {
                    sf::RectangleShape overlay(sf::Vector2f(960, 800));
                    overlay.setFillColor(sf::Color(0, 0, 0, 128));
                    window.draw(overlay);

                    window.draw(gameOverText);
                    window.draw(restartText);
                }
            }
        }
        else if (gameState == GameState::About) {
            sf::RenderWindow helpWindow(sf::VideoMode(800, 600), "Help", sf::Style::Titlebar | sf::Style::Close);
            helpWindow.setPosition(sf::Vector2i(window.getPosition().x + 250, window.getPosition().y + 100));

            while (helpWindow.isOpen()) {
                sf::Event helpEvent;
                while (helpWindow.pollEvent(helpEvent)) {
                    if (helpEvent.type == sf::Event::Closed) {
                        helpWindow.close();
                        gameState = GameState::Menu;
                    }
                    if (helpEvent.type == sf::Event::MouseButtonPressed && helpEvent.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(helpWindow);
                        sf::FloatRect backButtonBounds(350, 500, 100, 40);
                        if (backButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                            helpWindow.close();
                            gameState = GameState::Menu;
                        }
                    }
                    if (helpEvent.type == sf::Event::MouseMoved) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(helpWindow);
                        sf::FloatRect backButtonBounds(350, 500, 100, 40);
                        backButtonText.setFillColor(backButtonBounds.contains(static_cast<sf::Vector2f>(mousePos)) ? sf::Color::Yellow : sf::Color::White);
                    }
                }

                helpWindow.clear(sf::Color(50, 50, 50));
                sf::Vertex helpGradient[] = {
                    sf::Vertex(sf::Vector2f(0, 0), sf::Color(60, 60, 60)),
                    sf::Vertex(sf::Vector2f(800, 0), sf::Color(80, 80, 80)),
                    sf::Vertex(sf::Vector2f(0, 600), sf::Color(60, 60, 60)),
                    sf::Vertex(sf::Vector2f(800, 600), sf::Color(80, 80, 80))
                };
                helpWindow.draw(helpGradient, 4, sf::Quads);

                if (fontLoaded) {
                    sf::FloatRect titleBounds = helpTitle.getLocalBounds();
                    helpTitle.setPosition((800 - titleBounds.width) / 2, 20);
                    helpWindow.draw(helpTitle);

                    sf::RectangleShape textBox(sf::Vector2f(700, 400));
                    textBox.setPosition(50, 80);
                    textBox.setFillColor(sf::Color(240, 240, 240, 200));
                    textBox.setOutlineColor(sf::Color(100, 100, 100));
                    textBox.setOutlineThickness(2);
                    helpWindow.draw(textBox);

                    sf::FloatRect textBounds = helpText.getLocalBounds();
                    helpText.setPosition(70, 100);
                    helpWindow.draw(helpText);

                    sf::RectangleShape backButton(sf::Vector2f(100, 40));
                    backButton.setPosition(350, 500);
                    backButton.setFillColor(sf::Color(100, 100, 100));
                    backButton.setOutlineColor(sf::Color(150, 150, 150));
                    backButton.setOutlineThickness(2);
                    helpWindow.draw(backButton);

                    sf::FloatRect buttonTextBounds = backButtonText.getLocalBounds();
                    backButtonText.setPosition(350 + (100 - buttonTextBounds.width) / 2, 505);
                    helpWindow.draw(backButtonText);
                }

                helpWindow.display();
            }
        }

        window.display();
    }

    return 0;
}