
2-Player Snake Instructions

Set-up Steps:

YOU MUST USE C++17 or above

    1.) Install SFML version 2.5 and add to environment variables: URL = https://www.sfml-dev.org

    2.) Create a build folder in the project directory: COMMAND = mkdir build

    3.) Navigate to the build directory: COMMAND = cd build

    4.) build the executable with the CMakeList.txt inside of the build directory: COMMAND = cmake ..

    5.) COMMAND = cmake --build .

    6.) Run the executable from the build directory: COMMAND = ./PlaySnake


Playing the game:

    1.) Use WASD keys for movement. (W = up, A = left, S = down, D = right) NOTE: You cannot move 180 degrees, you must move 90 degrees first.

    2.) When game is over, close the window or wait for 5 sec. timeout.

    3.) Re-run the executable from terminal to replay.

