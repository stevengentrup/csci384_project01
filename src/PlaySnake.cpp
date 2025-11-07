// Steven Gentrup & Joseph Elsegeiny
// CSCI 384
// Project 1: 2 Player Snake
// Due: 11/09/2025

// To link the libraries in terminal: g++ PlaySnake.cpp -o PlaySnake -lsfml-graphics -lsfml-window -lsfml-system
#include "PlaySnake.h"

PlaySnake::PlaySnake()
    :aiController(numRows, numCols, playerTwo)
{
    // Window Creation
    sf::VideoMode mode;
    mode = sf::VideoMode(960, 700);
    window.create(mode, "2-Player Snake Game");

    screenWidth = mode.width;
    screenHeight = mode.height - 40; // For score at bottom of screen
    cellSize = std::min(screenWidth / numCols, screenHeight / numRows);

    // Initialize font for score
    if (!font.loadFromFile("../Fonts/DejaVuSans.ttf")) {
        std::cerr << "Could not load font\n";
    }

    // Initialize PlayerOne - Human
    playerOne.color = sf::Color::Green;
    playerOne.name = "Human Player";
    playerOne.isAI = false;
    playerOne.body.emplace_back(numCols / 2, numRows / 2);// middle of screen
    playerOne.body.emplace_back(numCols / 2, (numRows / 2) + 1);
    playerOne.body.emplace_back(numCols / 2, (numRows / 2) + 2);
    playerOne.direction = {0, -1};

    // Initialize playerTwo - AI
    playerTwo.color = sf::Color::Yellow;
    playerTwo.name = "AI Player";
    playerTwo.isAI = true;
    playerTwo.body.emplace_back(numCols / 4, numRows / 4);
    playerTwo.body.emplace_back(numCols / 4, (numRows / 4) + 1);
    playerTwo.body.emplace_back(numCols / 4, (numRows / 4) + 2);

    // Spawn initial food
    spawnFood();
}

// Game Loop
void PlaySnake::run() {
    while (window.isOpen()) {
        if (!isGameOver) {
            elapsedTime = clock.restart().asSeconds();
            processInput( window);
            update();
            render();
        }
        else {
            setWinner();
            clock.restart().asSeconds();
            while (clock.getElapsedTime().asSeconds() < 5.0f) {
                render();
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed){window.close();}
                }
            }
            window.close();
        }
    }
}

void PlaySnake::processInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) { // Removes single event from event queue each iteration
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && playerOne.direction.y != 1)
        playerOne.direction = {0, -1};
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && playerOne.direction.y != -1)
        playerOne.direction = {0, 1};
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && playerOne.direction.x != 1)
        playerOne.direction = {-1, 0};
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && playerOne.direction.x != -1)
        playerOne.direction = {1, 0};
}

void PlaySnake::update() {
    moveTimer += elapsedTime;
    // Add new head based on mouse position, checking for collisions, and determining if food has been eaten or not
    while (moveTimer >= moveInterval) { // If there is a fps drop, then we need to catch up on the missed moves.
        moveSnake(playerOne);
        moveSnake(playerTwo);

        checkCollision(playerOne, playerTwo);
        checkCollision(playerTwo, playerOne);

        moveTimer -= moveInterval;
    }
}

void PlaySnake::moveSnake(Snake& snake) {
    bool newHead = false;
    if (snake.isAI == false) {
        // Adjust all the snake segments in the array
        sf::Vector2i newHeadPos = snake.body.front() + snake.direction;
        snake.body.insert(snake.body.begin(), newHeadPos);
        newHead = true;
    }

    else {
        sf::Vector2i currHead = snake.body.front();
        sf::Vector2i newHeadPos = aiController.getNextMove(snake, playerOne, food);
        if (newHeadPos != currHead) {newHead = true;}
        snake.body.insert(snake.body.begin(), newHeadPos);
        if (!newHead) {
            snake.body.erase(snake.body.begin() + 1); // If the new move is same as current head,
        }
    }
    if (newHead == true) {
        if (snake.growthRemaining > 0) {
            snake.growthRemaining--;
        }
        else {
            snake.body.pop_back();
        }
    }
}

void PlaySnake::setWinner() {
    if (playerOne.penalized){winner = playerTwo;}
    if (playerTwo.penalized){winner = playerOne;}

    if (!playerOne.penalized && !playerTwo.penalized) {
        if (playerOne.score > playerTwo.score){winner = playerOne;}
        else {winner = playerTwo;}
    }
}

// Spawn food items on the board at the beginning of the game.
void PlaySnake::spawnFood() {
    food.clear(); // clear out when calling this method since we are only calling it once. Need it to start empty each time it is called.

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> distX(0, numCols - 1);
    std::uniform_int_distribution<int> distY(0, numRows - 1);
    for (int i = 0; i < 200; ++i) {
        sf::Vector2i newFood = sf::Vector2i(distX(rng), distY(rng));
        food.push_back(newFood);
    }
}

void PlaySnake::checkCollision(Snake &snake, Snake &otherSnake) { // Don't want to alter otherSnake so const
    sf::Vector2i currHead = snake.body.front(); // Need below line bc currHead is Read Only
    sf::Vector2i &head = snake.body.front();

    if (food.empty()) {
        isGameOver = true;
        return;
    }

    if (head.x < 0 || head.x > numCols) { // Check if snake out of bounds on x-axis
        std::cout << "SNAKE OOB ON X-AXIS\n";
        snake.penalized = true;
        isGameOver = true;
        return;
    }

    else if (head.y < 0 || head.y > numRows) { // Check if snake out of bounds on y-axis
        std::cout << "SNAKE OOB ON Y-AXIS\n";
        snake.penalized = true;
        isGameOver = true;
        return;
    }
    // Using size_t instead of int because vector.size() returns size_t
    // Check if snake has bitten itself
    if (snake.body.size() >= 4) { // Can't bite itself until length 4
        for (size_t i = 1; i < snake.body.size(); ++i) {
            if (snake.body[i] == currHead) {
                std::cout << "SNAKE BIT ITSELF\n";
                isGameOver = true;
                snake.penalized = true;
                return;
            }
        }
    }

    for (const auto seg : otherSnake.body) { // Check if snake has bitten the opponent snake
        if (seg == currHead) {
            std::cout << "SNAKE BIT OTHER SNAKE\n";
            isGameOver = true;
            snake.penalized = true;
            return;
        }
    }

    for (size_t i = 0; i < food.size(); ++i) { // Check if snake has eaten food
        int xDist = std::abs(currHead.x - food[i].x);
        int yDist = std::abs(currHead.y - food[i].y);
        if (food[i] == currHead || xDist <= 1 && yDist <= 1) { // If snake on the food or within a cell from the food.
            snake.score += 1;
            snake.growthRemaining += 2; // Grow 2 segments when eating 1 food
            food.erase(food.begin() + static_cast<int>(i));
        }
    }
}

// Update the game board / display
void PlaySnake::render() {
    window.clear();
    renderSnakes(window, playerOne);
    renderSnakes(window, playerTwo);
    renderFood(window);
    renderScore(window);
    if (isGameOver) {
        renderEndGame(window, winner);
    }
    window.display();
}

void PlaySnake::renderFood(sf::RenderWindow &window) {
    for (const auto &f : food) {
        sf::CircleShape foodShape(cellSize / 2.0f);
        foodShape.setFillColor(foodColor);
        foodShape.setPosition(static_cast<float>(f.x) * cellSize + cellSize / 2.0f,
            static_cast<float>(f.y) * cellSize + cellSize / 2.0f);
        window.draw(foodShape);
    }
}

void PlaySnake::renderSnakes(sf::RenderWindow& window, const Snake& snake){
    sf::CircleShape segShape(cellSize);
    segShape.setFillColor(snake.color);
    for (const auto &seg : snake.body) {
        segShape.setPosition(static_cast<float>(seg.x) * cellSize, static_cast<float>(seg.y) * cellSize);
        window.draw(segShape);
    }
}

void PlaySnake::renderScore(sf::RenderWindow& window) {
    sf::Text p1Text;
    sf::Text p2Text;

    p1Text.setFont(font);
    p1Text.setCharacterSize(20);
    p1Text.setFillColor(playerOne.color);
    p1Text.setPosition(20.f, 670); // Below the board
    p1Text.setString("Human Player Length: " + std::to_string(playerOne.score));
    window.draw(p1Text);

    p2Text.setFont(font);
    p2Text.setCharacterSize(20);
    p2Text.setFillColor(playerTwo.color);
    p2Text.setPosition(740.f, 670);
    p2Text.setString("AI Player Length: " + std::to_string(playerTwo.score));
    window.draw(p2Text);
}

void PlaySnake::renderEndGame(sf::RenderWindow &window, Snake &winningSnake) {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(60);
    text.setFillColor(sf::Color::Blue);
    text.setPosition((screenWidth / 4), (screenHeight / 4));
    text.setString(winner.name  + " Wins!\nScore: " + std::to_string(winner.score));
    window.draw(text);
}

int main() {
    PlaySnake curr_game;
    curr_game.run();
    return 0;
}