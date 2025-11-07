#ifndef SNAKE_H
#define SNAKE_H

#include <SFML/Graphics.hpp>
#include <vector>

struct Snake{
    std::vector<sf::Vector2i> body; // 2D vector array of x, y coordinates - This is the position of each segment of the snake's body. Vector2f will allow the snake to follow the mouse and move smoothly
    sf::Vector2i direction; // Movement vector
    sf::Color color;
    std::string name;
    int growthRemaining = 0;
    int score = 0;
    bool isAI; // Is this snake a player or AI
    bool penalized = false;
};

#endif //SNAKE_H
