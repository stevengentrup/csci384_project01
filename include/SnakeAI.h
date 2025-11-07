

#ifndef SNAKEAI_H
#define SNAKEAI_H

#include "Snake.h"
#include <queue>
#include <unordered_set>
#include <unordered_map>

// In order to use unordered_map and unordered_set with sf::Vector2i
struct Vector2iHash {
    std::size_t operator () (const sf::Vector2i& vec) const noexcept {
        std::size_t hashX = std::hash<int>{}(vec.x);
        std::size_t hashY = std::hash<int>{}(vec.y);
        return hashX ^ (hashY << 1);
    }
};

struct Node {
    sf::Vector2i pos;
    int g_cost;
    int h_cost;
    int f_cost;

    Node(sf::Vector2i pos, int g, int h)
        : pos(pos), g_cost(g), h_cost(h), f_cost(g_cost + h_cost) {}

    // std::priority_queue is a max heap, so we want to overload the < operator so that smaller values are first out.
    bool operator < (const Node& other) const {return this->f_cost > other.f_cost;}
};

class SnakeAI {
public:
    SnakeAI(int numRows, int numCols, Snake& aiSnake);


    sf::Vector2i getNextMove( // The method that will be called in the PlaySnake class
        Snake& aiSnake,
        const Snake& humanSnake,
        const std::vector<sf::Vector2i>& foodPos
    );

private:
    Snake aiSnake;
    int numRows;
    int numCols;

    std::priority_queue<Node> frontier; // Nodes to be expanded
    std::unordered_set<sf::Vector2i, Vector2iHash> explored; // Coordinates already explored
    std::unordered_map<sf::Vector2i, sf::Vector2i, Vector2iHash> tracePath; // Recreate the path of the goal node -- Key = Current, Val = Parent

    sf::Vector2i getClosestFood(const sf::Vector2i& head, const std::vector<sf::Vector2i>& foodPos);

    int calculateHCost(const sf::Vector2i& curr, const sf::Vector2i& goal);

    std::vector<sf::Vector2i> getNeighbors(
        const sf::Vector2i& position,
        const std::vector<sf::Vector2i>& obstacles
    );

    std::vector<sf::Vector2i> reconstructPath(const sf::Vector2i& start, const sf::Vector2i& goal);

    std::vector<sf::Vector2i> findPath(
        const sf::Vector2i& start, // AI snake head
        const sf::Vector2i& goal, // Nearest food pos
        const std::vector<sf::Vector2i>& obstacles // Both snake bodies excluding the AI Snake head
    );
};

#endif //SNAKEAI_H
