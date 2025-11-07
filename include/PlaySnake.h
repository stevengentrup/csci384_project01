#ifndef PLAYSNAKE_H
#define PLAYSNAKE_H

#include "Snake.h"
#include "SnakeAI.h"
#include <SFML/Window.hpp>   // SFML window & input handling
#include <SFML/System.hpp>   // SFML Clock, Vector2f
#include <cstdlib>           // for rand() / srand()
#include <ctime>             // for seeding rand()
#include <cmath>             // for sqrt() for vector normalization
#include <iostream>
#include <random>

  class PlaySnake {
  public:
    PlaySnake();
    void run();

  private:
    void processInput(sf::RenderWindow& window);
    void update();
    void render();

    void moveSnake(Snake& snake);
    void spawnFood();
    void checkCollision(Snake& snake, Snake& otherSnake);
    void setWinner();

    void renderSnakes(sf::RenderWindow& window, const Snake& snake);
    void renderFood(sf::RenderWindow& window);
    void renderScore(sf::RenderWindow& window);
    void renderEndGame(sf::RenderWindow& window, Snake& winningSnake);

    sf::RenderWindow window;
    sf::Font font;
    float cellSize;
    float screenWidth;
    float screenHeight;
    int numCols = 120;
    int numRows = 80;

    // Snakes
    Snake playerOne; // Human player
    Snake playerTwo; // AI player
    Snake winner;

    // AI class
    SnakeAI aiController;

    std::vector<sf::Vector2i> food; // Array for food
    sf::Color foodColor = sf::Color::Red;
    sf::Clock clock;
    float moveTimer = 0.0f;
    float moveInterval = 1.0f / 30.0f;
    float elapsedTime = 0.0f;

    bool isGameOver = false;
};
#endif //PLAYSNAKE_H
