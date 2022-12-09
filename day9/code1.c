#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


// Define max line length
#ifndef MAX_LINE_LEN
    #include <limits.h>
    #define MAX_LINE_LEN UCHAR_MAX
#endif


// Type definition for a knot
typedef struct Knot {
    long x;
    long y;
} knot_t;


// Type definition for the rope position
typedef struct RopePosition {
    long x;
    long y;
    unsigned long enter_count;
    unsigned long spend_count;
} rope_pos_t;


// Type definition for the rope tail position list
typedef struct RopePositionList {
    unsigned long num_positions;
    rope_pos_t **positions;
} rope_pos_list_t;


// Type definitions for enumerations of move alignment and direction
typedef enum {HORIZONTAL, VERTICAL} alignment_t;
typedef enum {FORWARD, BACKWARD} direction_t;

// Type definition for move instructions

typedef struct MoveInstruction {
    alignment_t alignment;
    direction_t direction;
    unsigned long num_steps;
} move_inst_t;


/// @brief Parse a file line for the instruction
/// @param file_line The line to parse
/// @return The instruction given by the file line
move_inst_t parse_instruction(char *file_line) {

    char *end_ptr;

    // Split the string into the direction and amount information
    char *direction_str = strtok(file_line, " ");
    char *num_steps_str = strtok(NULL, "\n");

    // Assign the alignment and direction depending on the instruction direction information
    alignment_t alignment;
    direction_t direction;
    switch (direction_str[0]) {
        case 'R':
            alignment = HORIZONTAL;
            direction = FORWARD;
            break;
        case 'L':
            alignment = HORIZONTAL;
            direction = BACKWARD;
            break;
        case 'D':
            alignment = VERTICAL;
            direction = FORWARD;
            break;
        case 'U':
            alignment = VERTICAL;
            direction = BACKWARD;
            break;
        default:
            printf("Error occurred while parsing instruction from file!");
            exit(1);
    }

    // Create, initialize, and return the next move
    move_inst_t next_move;
    next_move.alignment = alignment;
    next_move.direction = direction;
    next_move.num_steps = strtoul(num_steps_str, &end_ptr, 10);
    return next_move;

}


/// @brief Record the tail position
/// @param tail Pointer to the tail
/// @param position_list Pointer to the position list
/// @param tail_moved Whether the tail moved to a new location on this move
void record_tail_position(knot_t *tail, rope_pos_list_t *position_list, bool tail_moved) {
    
    // Add tail position to counted list if already there
    for (unsigned long index = 0; index < position_list->num_positions; index++) {
        rope_pos_t *saved_position = position_list->positions[index];
        if (tail->x == saved_position->x && tail->y == saved_position->y) {
            saved_position->spend_count++;
            saved_position->enter_count += tail_moved;
            return;
        }
    }

    // Increment the counter of positions if none was found
    position_list->num_positions++;

    // Allocate (or reallocate) memory for the new psotion
    if (position_list->num_positions == 1) {
        position_list->positions = malloc(sizeof(rope_pos_t *));
    }
    else {
        position_list->positions = realloc(position_list->positions, position_list->num_positions * sizeof(rope_pos_t *));
    }

    // Allocate memory for the new position and initialize it
    rope_pos_t *new_position = malloc(sizeof(rope_pos_t));
    new_position->x = tail->x;
    new_position->y = tail->y;
    new_position->enter_count = 1;
    new_position->spend_count = 1;

    // Add the position to the position list
    position_list->positions[position_list->num_positions - 1] = new_position;

}


/// @brief Move the rope tail if needed
/// @param tail Pointer to the tail
/// @param head Pointer to the head
/// @return Whether the tail moved to a new location
bool move_knot(knot_t *tail, knot_t *head) {

    // Keep track of whether the tail has moved
    bool tail_moved = false;

    // Get the sum of x and y distances (to check for diagonal moves)
    unsigned int block_distance = abs(head->x - tail->x) + abs(head->y - tail->y);

    // Check if tail needs to move in x direction
    if (block_distance > 2 || abs(head->x - tail->x) > 1) {
        if (head->x - tail->x > 0) { tail->x++; }
        else { tail->x--; }
        tail_moved = true;
    }

    // Check if tail needs to move in y direction
    if (block_distance > 2 || abs(head->y - tail->y) > 1) {
        if (head->y - tail->y > 0) { tail->y++; }
        else { tail->y--; }
        tail_moved = true;
    }

    // Return whether the tail has moved into a new square
    return tail_moved;

}


/// @brief Move the rope head as part of a single step within an instruction
/// @param head Pointer to the head
/// @param instruction The instruction of which this step is a part
void move_rope_head(knot_t *head, move_inst_t instruction) {
    int increment = instruction.direction == FORWARD ? 1 : -1;
    if (instruction.alignment == HORIZONTAL) { head->x += increment; }
    else { head->y += increment; }
}


/// @brief Execute a single move from an instruction for the rope
/// @param head Pointer to the head
/// @param tail Pointer to the tail
/// @param position_list Pointer to the position list
/// @param instruction The current instruction of which this step is a part
void execute_step(knot_t *head, knot_t *tail, rope_pos_list_t *position_list, move_inst_t instruction) {
    move_rope_head(head, instruction);
    bool tail_moved = move_knot(tail, head);
    record_tail_position(tail, position_list, tail_moved);
}


/// @brief Execute a single instruction for the rope
/// @param instruction The instruction to execute
/// @param head Pointer to the head
/// @param tail Pointer to the tail
/// @param position_list Pointer to the position list
void execute_instruction(move_inst_t instruction, knot_t *head, knot_t *tail, rope_pos_list_t *position_list) {
    for (long move_index = 0; move_index < instruction.num_steps; move_index++) {
        execute_step(head, tail, position_list, instruction);
    }
}


/// @brief The main function
/// @param argc The number of arguments given (1)
/// @param argv The arguments provided ([Program name, Input filepath])
/// @return The resulting exit code
int main(int argc, char **argv) {

    // Declare array for reading file
    char file_line[MAX_LINE_LEN];

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Initialize rope positions
    knot_t head = {0, 0};
    knot_t tail = {0, 0};

    // Initialize variable for storing positions
    rope_pos_t *positions;
    rope_pos_list_t position_list = {0, &positions};

    // Iterate through file
    while (true) {

        // Read a line from the file
        fgets(file_line, MAX_LINE_LEN, fp);

        // Break if end of file reached
        if (feof(fp)) { break; }

        // Get the next instruction
        move_inst_t next_instruction = parse_instruction(file_line);

        // Execute the instruction
        execute_instruction(next_instruction, &head, &tail, &position_list);

    }

    // Print the result
    printf("Unique locations visited: %lu\n", position_list.num_positions);

    // Return exit code 0 for success
    return 0;

}
