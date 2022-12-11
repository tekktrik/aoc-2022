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


// Type definition for the structure store information about cycles to track
typedef struct Cycles{
    unsigned int num_cycles;
    unsigned int *cycles;
} cycles_t;


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
/// @param cycles The cycles object containing information about which cycles to track
/// @param signal_strength Pointer to the signal strength
void execute_cycle(signal_t *signal, cycles_t cycles, unsigned long *signal_strength) {
    signal->cycle_num++;
    for (int index = 0; index < cycles.num_cycles; index++) {
        if (signal->cycle_num == cycles.cycles[index]) {
            *signal_strength = *signal_strength + (signal->cycle_num * signal->value);
        }
    }
}


/// @brief Execute an instruction
/// @param instruction The instruction to execute
/// @param signal Pointer to the signal object
/// @param cycles The cycles object containing information about which cycles to track
/// @param signal_strength Pointer to the signal strength
void execute_instruction(inst_t instruction, signal_t *signal, cycles_t cycles, unsigned long *signal_strength) {
    for (int noop_index = 0; noop_index < instruction.noop_cycles; noop_index++) {
        execute_cycle(signal, cycles, signal_strength);
    }
    signal->value += instruction.value;
}


int main(int argc, char **argv) {

    // Allocate memory for reading file
    char file_line[MAX_LINE_LEN];

    // Get the memory address and count for desired cycles
    char **cycles_str = &argv[2];
    char *end_ptr;
    unsigned int *cycles_array = malloc((argc - 2) * sizeof(unsigned int));
    for (int index = 0; index < argc - 2; index++) {
        cycles_array[index] = strtoul(cycles_str[index], &end_ptr, 10);
    }
    cycles_t cycles = {argc - 2, cycles_array};

    // Allocate memory for the signal and signal strength
    signal_t signal = {0, 1};
    unsigned long signal_strength = 0;

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
        execute_instruction(instruction, &signal, cycles, &signal_strength);

    }

    // Print result
    printf("Signal strength: %lu\n", signal_strength);

    // Return exit code 0 for success
    return 0;
}