# C++ Chess Engine

A custom-built chess engine written from scratch in C++. This project served as a platform for exploring bit-array-based board representation, move generation, and basic chess AI algorithms—the Negamax algorithm. Additionally, the program includes a simple, interactive graphical interface built in SFML.

## Features

* **Board Representation:** Uses 64-bit integers (bitboards) to speed up all operations.
* **Move Generation:** The program generates a table of all pseudo-legal moves and masks for sliding pieces. These are used to speed up the process of finding all legal moves without having to generate possible attack squares every time it's needed.
* **Search Algorithm:** Uses the Negamax algorithm combined with the alpha-beta algorithm to explore the game tree and find the best move.
* **Position Evaluation:** Includes an evaluation function that uses material weights and piece tables, encouraging the program to control the center, develop pieces, and penalize duplicate pieces.
* **Graphical Interface:** A playable user interface - SFML library- allows the player to directly interact and compete with the engine.

## Project Structure

The code is divided into several components:

* **`main.cpp`**
The application's starting point. It deales with the SFML window, renders the chessboard and pieces, handles the user's mouse movements, and manages the main game loop between the user and the engine.


*   **`types.h`**
    Contains essential enumerations and types used throughout the engine, including `Color`, `PieceType`, `Square`, and a highly optimized 16-bit integer representation for moves (encoding source square, target square, and special flags) 

*   **`bitboard.cpp` & `bitboard.h`**
    The bitboard.cpp and bitboard.h files define enums for key variables, making code easier to work with and read. They create the board's structure and set its initial parameters. They also provide information about the board's position, for example, through a function that returns the indexes of occupied squares.

*   **`attacks.cpp` & `attacks.h`**
	These two files are responsible for predefining the attack tables for each figure. Additionally, for sliding figures, they create masks that allow for updating new attacks after a move.

*   **`move_generator.cpp`** & **`move_generator.h`**
    Iterates over the bitboards to generate all pseudo-legal moves for the current side to move. 

*   **`make_move.cpp`** & **`make_move.h`**
    These files execute a given move, changing the bitboards and the game state. Additionally, they check the final legality of the move—whether the king has been exposed to attack.

*   **`evaluate_position.cpp`** & **`evaluate_position.h`**
    Static evaluation module: Calculates the total score for the current position based on material advantage, mid-game/endgame setup and pawn structure penalties. 

*   **`search.cpp`** & **`search.h`**
    The core "brain" of the engine. It uses negamax with alpha-beta pruning to look ahead into future moves up to a specified depth. It returns the most optimal move found 

## Dependencies

*   A modern C++ Compiler.
*   **SFML (Multimedia Library):  Required for rendering the graphics window and handling user input.
