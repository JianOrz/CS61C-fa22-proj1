#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  game_state_t* default_state = malloc(sizeof(game_state_t));
  if (default_state == NULL) {
    fprintf(stderr, "Out of memory.\n");
    exit(-1);
  }
  default_state->num_rows = 18;
  default_state->num_snakes = 1;
  default_state->board = malloc(default_state->num_rows * sizeof(char*));
  if (default_state->board == NULL) {
    free(default_state);
    fprintf(stderr, "Out of memory.\n");
    exit(-1);
  }
  default_state->snakes = malloc(default_state->num_snakes * sizeof(snake_t));
  if (default_state->snakes == NULL) {
    free(default_state->board);
    free(default_state);
    fprintf(stderr, "Out of memory.\n");
    exit(-1);
  }

  for (int i = 0; i < 18; i++) {
    default_state->board[i] = malloc(21 * sizeof(char));
  }
  strcpy(default_state->board[0], "####################");
  strcpy(default_state->board[17], "####################");
  for (int i = 1; i < 17; i++) {
    strcpy(default_state->board[i], "#                  #");
  }
  strcpy(default_state->board[2],"# d>D    *         #");
  default_state->snakes->head_col = 4;
  default_state->snakes->head_row = 2;
  default_state->snakes->tail_col = 2;
  default_state->snakes->tail_row = 2;
  default_state->snakes->live = true;
  return default_state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.

  for (int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  if (fp == NULL) {
    exit(-1);
  } 
  unsigned int rows = state->num_rows;

  for (unsigned int i = 0; i < rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }

  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if (c == 'a' || c == 'w' || c == 's' || c == 'd') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if (is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>') {
    return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch (c)
  {
  case '^':
    return 'w';
  case '<':
    return 'a';
  case 'v':
    return 's';
  case '>':
    return 'd';
  default:
    return '?';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch (c)
  {
  case 'W':
    return '^';
  case 'A':
    return '<';
  case 'S':
    return 'v';
  case 'D':
    return '>';
  default:
    return '?';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  if (!state->snakes[snum].live) {
    return '?';
  }
  unsigned int head_col = state->snakes[snum].head_col;
  unsigned int head_row = state->snakes[snum].head_row;
  char head = get_board_at(state, head_row, head_col);
  unsigned int next_row = get_next_row(head_row, head);
  unsigned int next_col = get_next_col(head_col, head);
  return get_board_at(state, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  if (!state->snakes[snum].live) {
    return;
  }

  unsigned int head_col = state->snakes[snum].head_col;
  unsigned int head_row = state->snakes[snum].head_row;
  char head = get_board_at(state, head_row, head_col);
  char body = head_to_body(head);
  set_board_at(state, head_row, head_col, body);
  unsigned int next_row = get_next_row(head_row, head);
  unsigned int next_col = get_next_col(head_col, head);
  set_board_at(state, next_row, next_col, head);
  state->snakes[snum].head_col = next_col;
  state->snakes[snum].head_row = next_row;
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  
  if (!state->snakes[snum].live) {
    return;
  }
  unsigned int tail_row = state->snakes[snum].tail_row;
  unsigned int tail_col = state->snakes[snum].tail_col;
  char tail = get_board_at(state, tail_row, tail_col);
  set_board_at(state, tail_row, tail_col, ' '); 
  unsigned int next_row = get_next_row(tail_row, tail);
  unsigned int next_col = get_next_col(tail_col, tail);
  char body = get_board_at(state, next_row, next_col);
  char new_tail = body_to_tail(body);
  set_board_at(state, next_row, next_col, new_tail);
  state->snakes[snum].tail_row = next_row;
  state->snakes[snum].tail_col = next_col;
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.

  for (unsigned int i = 0; i < state->num_snakes; i++) {
    if (!state->snakes[i].live) {
      continue;
    }
    char next_char = next_square(state, i);
    if (next_char == ' ') {
      update_head(state, i);
      update_tail(state, i);
    } else if (next_char == '*') {
      update_head(state, i);
      add_food(state);
    } else {
      unsigned int head_col = state->snakes[i].head_col;
      unsigned int head_row = state->snakes[i].head_row;
      set_board_at(state, head_row, head_col, 'x');
      state->snakes[i].live = false;
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
  game_state_t* state = malloc(sizeof(game_state_t));
  if (state == NULL) {
    fprintf(stderr, "Out of memory!");
    exit(-1);
  }
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    free(state);
    exit(-1);
  }
  char ch;
  unsigned int cnt = 0;
  while (!feof(fp)) {
    ch = fgetc(fp);
    if (ch == '\n') {
      cnt++;
    }
  } 
  state->num_rows = cnt;
  state->board = malloc(state->num_rows * sizeof(char*));
  if (state->board == NULL) {
    free(state);
    fprintf(stderr, "Out of memory!");
    exit(-1);
  }
  rewind(fp);
  char str[1024 * 1024];
  int i = 0;
  while(fgets(str, 1024 * 1024, fp)) {
    state->board[i] = malloc(strlen(str) * sizeof(char));
    strncpy(state->board[i], str, strlen(str) - 1);
    state->board[i][strlen(str) - 1] = '\0';
    i++;
  }
  fclose(fp);
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.

  unsigned int cur_row = state->snakes[snum].tail_row;
  unsigned int cur_col = state->snakes[snum].tail_col;
  char ch = get_board_at(state, cur_row, cur_col);

  unsigned int next_row;
  unsigned int next_col;
  while (1) {
    next_row = get_next_row(cur_row, ch);
    next_col = get_next_col(cur_col, ch);
    ch = get_board_at(state, next_row, next_col);
    if (is_head(ch)) {
      state->snakes[snum].head_col = next_col;
      state->snakes[snum].head_row = next_row;
      return;
    }
    cur_row = next_row;
    cur_col = next_col; 
  }
  
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  unsigned int rows = state->num_rows;
  long unsigned int length;
  state->num_snakes = 0;
  for (int i = 0; i < rows; i++) {
    length = strlen(state->board[i]);
    for (int j = 0; j < length; j++) {
      if (is_tail(state->board[i][j])) {
        state->num_snakes++;
      }
    }
  }
  state->snakes = malloc(state->num_snakes * sizeof(snake_t));
  if (state->snakes == NULL) {
    fprintf(stderr, "Out of memory!");
    exit(-1);
  }
  for (unsigned int i = 0, snum = 0; i < rows; i++) {
    length = strlen(state->board[i]);
    for (unsigned int j = 0; j < length; j++) {
      if (is_tail(state->board[i][j])) {
        state->snakes[snum].live = true;
        state->snakes[snum].tail_row = i;
        state->snakes[snum].tail_col = j;
        find_head(state, snum);
        snum++;
      }
    }
  }
  return state;
}
