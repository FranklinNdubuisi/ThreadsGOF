My name is Franklin Ndubuisi. This is Operating System (CS3790) lab 4 assignment. This program simulates Conway's Game of Life using multithreading. Threads enable concurrent execution by dividing the grid into subregions, allowing each thread to process a portion of the grid independently. Ncurses is used to visualize the board, and users can manually configure the initial state of the simulation using mouse input.

To build the program, use the command g++ -pthread -lncurses -o threads <filename>.cpp. Run the program with ./threads <M> <N> <MAX>, ensuring that M is divisible by N.
