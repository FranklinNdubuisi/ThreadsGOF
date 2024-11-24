#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <ncurses.h>

std::vector<std::vector<int>> board;
int M, N, MAX;
std::mutex mtx;

// Function to count live neighbors of a cell
int getLiveNeighbors(int x, int y) {
    int liveNeighbors = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;  // Skip the cell itself

            int newX = x + i;
            int newY = y + j;
            if (newX >= 0 && newX < M && newY >= 0 && newY < M) {
                liveNeighbors += board[newX][newY];
            }
        }
    }
    return liveNeighbors;
}

// Thread function to compute the next generation for a subregion
void computeSubregion(int startRow, int endRow, int startCol, int endCol, std::vector<std::vector<int>>& newBoard) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startCol; j < endCol; ++j) {
            int liveNeighbors = getLiveNeighbors(i, j);

            if (board[i][j] == 1) {  // Cell is currently alive
                if (liveNeighbors < 2 || liveNeighbors > 3) {
                    newBoard[i][j] = 0;  // Cell dies
                }
                else {
                    newBoard[i][j] = 1;  // Cell survives
                }
            }
            else {  // Cell is currently dead
                if (liveNeighbors == 3) {
                    newBoard[i][j] = 1;  // Dead cell becomes alive
                }
            }
        }
    }
}

// Print the board using ncurses
void printGrid() {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < M; ++j) {
            mvprintw(i, j, board[i][j] == 1 ? "x" : " ");
        }
    }
    refresh();
}

// Input the glider gun 
void inputGliderGun() {
    MEVENT event;
    bool editing = true;
    printGrid();

    while (editing) {
        int c = getch();  // Wait for key press
        if (c == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                if (event.bstate & BUTTON1_PRESSED) {
                    if (event.y >= 0 && event.y < M && event.x >= 0 && event.x < M) {
                        board[event.y][event.x] = !board[event.y][event.x];  // Toggle cell state
                        printGrid();
                    }
                }
            }
        }
        else if (c == 'd') {
            editing = false;  // Finish editing when 'd' is pressed
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: ./threads <M> <N> <MAX>\n";
        return 1;
    }

    M = std::stoi(argv[1]);
    N = std::stoi(argv[2]);
    MAX = std::stoi(argv[3]);

    if (M % N != 0) {
        std::cerr << "Error: M must be divisible by N.\n";
        return 1;
    }

    // Initialize the board
    board = std::vector<std::vector<int>>(M, std::vector<int>(M, 0));

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);

    inputGliderGun();  // Input initial configuration

    // Calculate row and column regions
    int rowsPerThread = M / N;
    int colsPerThread = M / N;

    for (int generation = 0; generation < MAX; ++generation) {
        std::vector<std::thread> threads;
        std::vector<std::vector<int>> newBoard = board;

        // Create threads to compute subregions
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                int startRow = i * rowsPerThread;
                int endRow = startRow + rowsPerThread;
                int startCol = j * colsPerThread;
                int endCol = startCol + colsPerThread;

                threads.emplace_back(computeSubregion, startRow, endRow, startCol, endCol, std::ref(newBoard));
            }
        }

        // Join threads
        for (auto& t : threads) {
            t.join(); //waits for all threads to finish execution.
        }

        // Update the board
        board = newBoard;

        // Display the current generation
        printGrid();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    endwin();
    return 0;
}
