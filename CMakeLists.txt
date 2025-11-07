cmake_minimum_required(VERSION 3.10)

project(PlaySnake)

# Must use C++ 17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML 2.5 and up
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Source code files
add_executable(PlaySnake
        src/PlaySnake.cpp
        src/SnakeAI.cpp
)

# Where to find the headers
target_include_directories(PlaySnake PRIVATE include)


# Link SFML libraries
target_link_libraries(PlaySnake sfml-graphics sfml-window sfml-system)
