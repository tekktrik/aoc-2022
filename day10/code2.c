#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


// Define max line length
#ifndef MAX_LINE_LEN
    #include <limits.h>
    #define MAX_LINE_LEN UCHAR_MAX
#endif


// Type definitions for the types of operations
typedef enum {NOOP, ADDX} operation_t;


// Type definition for instructions
typedef struct Instruction {
    unsigned int inst_num;
    operation_t operation;
    long value;
    unsigned char noop_cycles;
} inst_t;


// Type definition for the current signal
typedef struct Signal {
    unsigned int cycle_num;
    long value;
} signal_t;


/// @brief Parse an instruction from the file
/// @param file_line The file line to parse
/// @return THe instruction represented
inst_t parse_instruction(char *file_line) {

    // Keep track of the instruction number
    static unsigned long inst_num = 1;

    // Default to setting up noop
    long op_value = 0;
    char *op_str = strtok(file_line, " ");
    operation_t operation = NOOP;
    unsigned char noop_cycles = 1;

    // Check if the string is "addx"
    if (!strcmp(op_str, "addx")) { // If addx
        char *end_ptr;
        op_value = strtol(strtok(NULL, "\n"), &end_ptr, 10);
        operation = ADDX;
        noop_cycles = 2;
    }

    // Create and return the resulting instruction (and also increment the instruction number)
    inst_t next_instruction = {inst_num, operation, op_value, noop_cycles};
    inst_num++;
    return next_instruction;
}


/// @brief Execute a single cycle
/// @param signal Pointer to the signal object
/// @param width The width of the CRT
/// @return Whether the current location overlaps with the sprite
bool execute_cycle(signal_t *signal, unsigned int width) {
    unsigned int row_position = signal->cycle_num % width;
    signal->cycle_num++;
    if (row_position >= signal->value - 1 && row_position <= signal->value + 1) {
        return true;
    }
    else {
        return false;
    }
}


/// @brief Display the characters on the CRT
/// @param pixel_state The pixel state array
/// @param height The height of the CRT
/// @param width The width of the CRT
void display_crt(unsigned int *pixel_state, unsigned int height, unsigned int width) {
    for (unsigned int row_index = 0; row_index < height; row_index++) {
        for (unsigned int col_index = 0; col_index < width; col_index++) {
            unsigned long position = (row_index * width) + col_index;
            if (pixel_state[position]) { printf("#"); }
            else { printf("."); }
        }
        printf("\n");
    }
}


/// @brief Execute an instruction
/// @param instruction The instruction to execute
/// @param signal Pointer to the signal object
/// @param pixel_state Pointer to the pixel state map
/// @param width The width of the CRT
void execute_instruction(inst_t instruction, signal_t *signal, unsigned int *pixel_state, unsigned int width) {
    for (int noop_index = 0; noop_index < instruction.noop_cycles; noop_index++) {
        if (execute_cycle(signal, width)) {
            pixel_state[signal->cycle_num - 1] = true;
        }
    }
    signal->value += instruction.value;
}


int main(int argc, char **argv) {

    // Allocate memory for reading file
    char file_line[MAX_LINE_LEN];

    // Allocate memory for the signal and signal strength
    signal_t signal = {0, 1};
    char *end_ptr;
    unsigned int height = strtoul(argv[2], &end_ptr, 10);
    unsigned int width = strtoul(argv[3], &end_ptr, 10);
    unsigned int *pixel_state = calloc(height * width, sizeof(int));

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Iterate through the file
    while (true) {

        // Read a line from the file
        fgets(file_line, sizeof(file_line), fp);

        // Break if end of file reached
        if (feof(fp)) { break; }

        // Parse the line into an instruction
        inst_t instruction = parse_instruction(file_line);

        // Execute the instrunction
        execute_instruction(instruction, &signal, pixel_state, width);

    }

    // Print the resulting pixel state map
    display_crt(pixel_state, height, width);

    // Return exit code 0 for success
    return 0;
}