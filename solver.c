#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Number of rows or columns on the board.
#define BOARD_SIZE 3

// Cell walls.
#define TOP 1
#define RIGHT 2
#define BOTTOM 4
#define LEFT 8

// Robot colors and their bit index within
// the long position.
#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define GRAY 5

// Bit mask for the given robot.
#define ROBOT_SHIFT(ROBOT)  ((8 * ROBOT) - 8)

// Mask for checking a cell.
#define CELL_MASK(CELL, ROBOT) (unsigned long)CELL << ROBOT_SHIFT(ROBOT)

// Returns the cell the given robot is at.
#define CELL_OF_ROBOT(ROBOT, POSITIONS) (POSITIONS >> ROBOT_SHIFT(ROBOT)) & 255

/*
 * Checks if the given destination cell contains the given robot using the robot
 * positions.
 */
uint8_t cell_contains_robot(uint8_t destination_cell, uint64_t robot_positions, uint8_t robot) {
  uint8_t cell_of_robot = CELL_OF_ROBOT(robot, robot_positions);

  return (cell_of_robot == destination_cell ? 1 : 0);
}

/*
 * Checks if the given destination cell contains any robot using the given robots and their
 * positions.
 */
uint8_t cell_contains_any_robot(uint8_t destination_cell, uint64_t robot_positions) {
  uint8_t robots[5] = {BLUE, GREEN, YELLOW, RED, GRAY};
  for (int i = 0; i < 5; i++) {
    if (cell_contains_robot(destination_cell, robot_positions, robots[i]) == 1) {
      return 1;
    }
  }

  return 0;
}

/*
 * Returns the cell a robot would end up in should he move upwards. If no move is
 * possible, the current cell is returned.
 */
uint8_t cell_up(uint8_t *board, uint8_t starting_cell, uint64_t robot_positions) {
  uint8_t current = starting_cell;
  if (starting_cell >= BOARD_SIZE) {
    uint8_t next = starting_cell - BOARD_SIZE;

    while (
        current >= 0 && 
        (board[current] & TOP) == 0 && 
        (board[next] & BOTTOM) == 0 && 
        cell_contains_any_robot(next, robot_positions) == 0
    ) {
      current = next;
      next -= BOARD_SIZE;
    }
  }

  return current; 
}

/*
 * Returns the cell a robot would end up in should he move right. If no move is
 * possible, the current cell is returned.
 */
uint8_t cell_right(uint8_t *board, uint8_t starting_cell, uint64_t robot_positions) {
  uint8_t current = starting_cell;
  if (starting_cell % BOARD_SIZE != (BOARD_SIZE - 1)) {
    uint8_t next = starting_cell + 1;

    while (
        current >= 0 && 
        (board[current] & RIGHT) == 0 && 
        (board[next] & LEFT) == 0 && 
        cell_contains_any_robot(next, robot_positions
    ) == 0) {
      current = next;
      next++;
    }
  }

  return current; 
}

/*
 * Returns the cell a robot would end up in should he move down. If no move is
 * possible, the current cell is returned.
 */
uint8_t cell_down(uint8_t *board, uint8_t starting_cell, uint64_t robot_positions) {
  uint8_t current = starting_cell;
  if ((starting_cell + BOARD_SIZE) < (BOARD_SIZE * BOARD_SIZE)) {
    uint8_t next = starting_cell + BOARD_SIZE;

    while (
        current < (BOARD_SIZE * BOARD_SIZE) && 
        (board[current] & BOTTOM) == 0 && 
        (board[next] & TOP) == 0 && 
        cell_contains_any_robot(next, robot_positions) == 0
    ) {
      current = next;
      next += BOARD_SIZE;
    }
  }

  return current; 
}

/*
 * Returns the cell a robot would end up in should he move left. If no move is
 * possible, the current cell is returned.
 */
uint8_t cell_left(uint8_t *board, uint8_t starting_cell, uint64_t robot_positions) {
  uint8_t current = starting_cell;
  if (starting_cell % BOARD_SIZE != 0) {
    uint8_t next = starting_cell - 1;

    while (
        current >= 0 && 
        (board[current] & LEFT) == 0 && 
        (board[next] & RIGHT) == 0 && 
        cell_contains_any_robot(next, robot_positions) == 0
    ) {
      current = next;
      next--;;
    }
  }

  return current; 
}

/*
 * Moves the given robot to the given destination cell by modifying
 * the robot positions. Returns the new robot positions.
 */
uint64_t move_robot(uint8_t robot, uint8_t destination_cell, uint64_t robot_positions) {
  // Clear the current cell of the robot.
  robot_positions &= ~(255UL << ROBOT_SHIFT(robot));

  // Set the new cell.
  uint64_t mask = CELL_MASK(destination_cell, robot);

  return robot_positions |= mask;
}

/*
 * Textual representation of moving a robot.
 */
void print_move(uint8_t robot, char *move) {
  if (robot == 1) {
    printf("Move red %s\n", move);
  } else if (robot == 2) {
    printf("Move green %s\n", move);
  } else if (robot == 3) {
    printf("Move blue %s\n", move);
  } else if (robot == 4) {
    printf("Move yellow %s\n", move);
  } else {
    printf("Move gray %s\n", move);
  }
}

/*
 * Tries to move the given robot to the given destination cell on the board containing
 * the given robots in the given positions using the given number of moves.
 */
uint8_t solve(uint8_t *board, uint8_t destination_cell, uint8_t robot, uint64_t robot_positions, uint8_t *solved, uint8_t moves) {
  // Is a solution already found?
  if (*solved == 1) {
    return 0;
  }

  // We're done if the robot is at the right position and we don't have any
  // moves left.
  if (moves == 0) {
    if (cell_contains_robot(destination_cell, robot_positions, robot) == 1) {
      *solved = 1;

      return 1;
    } else {

      return 0;
    }
  }

  // Recursively move all robots to all sides.
  uint8_t robots[5] = {BLUE, GREEN, YELLOW, RED, GRAY};
  for (int i = 0; i < 5; i++) {
    uint8_t current_robot = robots[i];
    uint8_t current_cell = CELL_OF_ROBOT(current_robot, robot_positions); 

    // There are potentially four directions a robot can move in.
    uint8_t (*move_functions[])(uint8_t *, uint8_t, uint64_t) = {
      cell_up, cell_right, cell_down, cell_left
    };

    for (int j = 0; j < 4; j++) {
      uint8_t new_cell = (*move_functions[j])(board, current_cell, robot_positions);

      if (new_cell != current_cell) {
        uint64_t new_robot_positions = move_robot(current_robot, new_cell, robot_positions);
        if (solve(board, destination_cell, robot, new_robot_positions, solved, moves - 1) == 1) {
          switch (j) {
            case 0:
              print_move(current_robot, "up");
              break;
            case 1:
              print_move(current_robot, "right");
              break;
            case 2:
              print_move(current_robot, "bottom");
              break;
            case 3:
              print_move(current_robot, "left");
              break;
          }
          return 1;
        }
      }
    }
  }

  return 0;
}

int main(void) {
  // Test board.
  uint8_t *board = malloc(sizeof(uint8_t) * BOARD_SIZE * BOARD_SIZE);

  // Top row.
  board[0] = 13;
  board[1] = 1;
  board[2] = 3;

  // Middle row.
  board[3] = 9;
  board[4] = 0;
  board[5] = 2;

  // Bottom row.
  board[6] = 14;
  board[7] = 4;
  board[8] = 6;

  uint64_t positions = 0;
  uint8_t destination_cell = 0;
  uint8_t robot = RED;
  uint8_t solved = 0;

  // Move the robots to the starting position.
  positions = move_robot(BLUE, 0, positions);
  positions = move_robot(GREEN, 1, positions);
  positions = move_robot(YELLOW, 8, positions);
  positions = move_robot(RED, 6, positions);
  positions = move_robot(GRAY, 4, positions);

  // Solve for increasing numbers of moves.
  uint8_t nr_moves = 1;
  while (solved == 0) {
    solve(board, destination_cell, robot, positions, &solved, nr_moves);
    nr_moves++;
  }

  free(board);

  return 0;
}

