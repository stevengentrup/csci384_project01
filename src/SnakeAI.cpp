#include "SnakeAI.h"

SnakeAI::SnakeAI(int numRows, int numCols, Snake& aiSnake)
    : numRows(numRows), numCols(numCols), aiSnake(aiSnake){} // No other initialization necessary in the body.

// Return closest food by distance to AI snake head
sf::Vector2i SnakeAI::getClosestFood(const sf::Vector2i& head, const std::vector<sf::Vector2i>& foodPos){
    sf::Vector2i closest = foodPos[0];
    int minDist = std::abs(head.x - closest.x) + std::abs(head.y - closest.y);

    for (const auto& food : foodPos){
        int dist = std::abs(head.x - food.x) + std::abs(head.y - food.y);
        if (dist < minDist){
            minDist = dist;
            closest = food;
        }
    }
    return closest;
}

// Return the h_cost for current position and goal postion for the Node. This is the Manhattan Distance and heuristic
int SnakeAI::calculateHCost(const sf::Vector2i& curr, const sf::Vector2i& goal){
    return std::abs(curr.x - goal.x) + std::abs(curr.y - goal.y);
}

std::vector<sf::Vector2i> SnakeAI::reconstructPath(const sf::Vector2i& start, const sf::Vector2i& goal){
	std::vector<sf::Vector2i> path;
    sf::Vector2i current = goal;

    while (current != start){
    	path.push_back(current);

        // Broken path safety check
        auto it = tracePath.find(current);
        if (it == tracePath.end()){
        	path.clear();
            return path;
        }
        current = it->second;
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end()); // current head (start) at path[0], next move at path[1]
    return path;
}

std::vector<sf::Vector2i> SnakeAI::findPath(
        const sf::Vector2i& start, // AI snake head
        const sf::Vector2i& goal, // Nearest food pos
        const std::vector<sf::Vector2i>& obstacles)
{
    // Clear out frontier, tracePath, and explored
    while (!frontier.empty()){frontier.pop();}
    tracePath.clear();
    explored.clear();

    // Add start node to frontier
    Node startNode(start, 0, calculateHCost(start, goal));
    frontier.push(startNode);

    // Loop while the frontier has nodes
    while (!frontier.empty()){
		Node current = frontier.top();
    	frontier.pop();

        if (current.pos == goal){
          return reconstructPath(start, current.pos);
        }

        // If node in explored, skip it
        if (explored.find(current.pos) != explored.end()){continue;}
        // Otherwise add it to explored
        explored.insert(current.pos);

        // Explore all neighbors
        for (const auto& neighborPos : getNeighbors(current.pos, obstacles)){
        	// Check if neighbor in explored
            if (explored.find(neighborPos) != explored.end()){continue;}

            int g = current.g_cost + 1;
            int h = calculateHCost(neighborPos, goal);
            Node neighbor(neighborPos, g, h);

            if (tracePath.find(neighborPos) == tracePath.end()){
              tracePath[neighborPos] = current.pos;
            }
            frontier.push(neighbor);
        }
    }
    return {}; // No path found, so return empty vector
}

sf::Vector2i SnakeAI::getNextMove(Snake& aiSnake, const Snake& humanSnake,
                                  const std::vector<sf::Vector2i>& foodPos)
{
    // Purpose:
    //             To return the coordinates of the next valid move to the PlaySnake::Update() method

    // Parameters:
    //            aiSnake: The entire body of the AI snake (including head)
    //            humanSnake: The entire body of the human snake (including head)
    //            foodPos: The vector array of coordinates where food is stored

    // Return value:
    //            The new position of the AI snake head

    sf::Vector2i start = aiSnake.body.front(); // Head of aiSnake
    sf::Vector2i goal = getClosestFood(start, foodPos); // The coordinates of the closest food to ai snake head

    // Initialize obstacles -- NOTE: Not adding board boundaries to obstacles, do a check in findPath() for OOB
    std::vector<sf::Vector2i> obstacles;

    // Add aiSnake body to obstacles
    obstacles.insert(obstacles.end(), aiSnake.body.begin(), aiSnake.body.end());

    // Add human snake to obstacles. Inserting before the outer range of the vector iterator, which effectively appends
    obstacles.insert(obstacles.end(), humanSnake.body.begin(), humanSnake.body.end());

    // Call findPath to get the next the set of moves to a food
    std::vector<sf::Vector2i> nextPath = findPath(start, goal, obstacles);
    sf::Vector2i nextMove = start;

    // Make sure nextPath is larger than 1
    if (nextPath.size() > 1 && nextPath[1] != start){
        nextMove = nextPath[1]; // front of the nextPath array is the current head. index 1 is the next position of the head
    }

    else {
        auto neighbors = getNeighbors(start, obstacles);
        auto dir = aiSnake.direction;
        for (const auto& neigh: neighbors) {
            // To make sure that the nextMove is never the current head and is not a 180 degree move
            if (neigh != start && neigh != start - dir) {
                nextMove = neigh;
                break;
            }
        }
        if (nextMove == start && !neighbors.empty()) {
            nextMove = neighbors[0]; // Pick first neighbor, do not return start or the snake will always bite itself.
        }
    }
    aiSnake.direction = start - nextMove;
    return nextMove;
}

std::vector<sf::Vector2i> SnakeAI::getNeighbors(
    const sf::Vector2i& position,
    const std::vector<sf::Vector2i>& obstacles){

    std::vector<sf::Vector2i> neighbors;

    const sf::Vector2i directions[4] = {
        {0, -1}, // Up
        {0, 1}, // Down
        {-1, 0}, // Left
        {1, 0} // Right
    };

    sf::Vector2i currentDir = aiSnake.direction;
    for (const auto& dir : directions){
        // Check that direction is not opposite direction (Cannot make a 180 degree turn)
        if (dir == -currentDir) {continue;}
        sf::Vector2i next = position + dir;

        // Check board bounds
        if (next.x < 0 || next.x > numCols || next.y < 0 || next.y > numRows){continue;}

        // Check if in obstacles
        bool tooClose = false;
        for (const auto& obstacle : obstacles) {
            int xDist = std::abs(next.x - obstacle.x);
            int yDist = std::abs(next.y - obstacle.y);
            if (xDist < 1 && yDist < 1) {
                tooClose = true;
                break;
            }
        }
        if (tooClose){continue;}

        neighbors.push_back(next);
    }
    return neighbors;
}
