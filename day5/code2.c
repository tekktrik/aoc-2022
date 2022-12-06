#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>

// Defintions
#define MAX_LINE_LENGTH UCHAR_MAX

// Type defintion for a box moving instruction
typedef struct instruction {
    unsigned int src;
    unsigned int dest;
    unsigned int num;
} inst_t;


/// @brief Get the number of stacks
/// @param fp The file pointer
/// @return The number of stacks
unsigned int getNumberOfStacks(FILE *fp) {

    // Get a buffer to store characters
    char fileLine[MAX_LINE_LENGTH];

    // Get current file pointer position and set to beginning
    unsigned long currentPosition = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Get the first line of the file
    fgets(fileLine, sizeof(fileLine), fp);

    // Return the pointer to the original position
    fseek(fp, currentPosition, SEEK_SET);

    // Return the number of stacks (4 characters per stack)
    return strlen(fileLine) / 4;

}


/// @brief Get the size of the current largest stack
/// @param fp The file pointer
/// @return The size of the current largest stack
unsigned int getCurrentMaxStack(FILE *fp) {

    // Get a buffer to store characters
    char fileLine[MAX_LINE_LENGTH];

    // Get current file pointer position and set to beginning
    unsigned long currentPosition = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Count the number of stacks now active
    unsigned int maxStackSize = 0;
    while (fgets(fileLine, MAX_LINE_LENGTH, fp)[0] != ' ') { maxStackSize++; }

    // Reset the file pointer position
    fseek(fp, currentPosition, SEEK_SET);

    // Return the current max stack size
    return maxStackSize;
    
}


/// @brief Initialize the stacks
/// @param fp The file pointer
/// @param stacks A pointer to store the stacks state information
/// @param numStacks The number of stacks
/// @param initialMaxStack The size of the current largest stack
void initializeStacks(FILE *fp, char **stacks, unsigned int numStacks, unsigned int initialMaxStack) {
    
    // Get current file pointer position and set to beginning
    unsigned long currentPosition = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    for (int stackIndex = 0; stackIndex < numStacks; stackIndex++) {
        stacks[stackIndex][initialMaxStack] = '\0';
        for (int heightIndex = initialMaxStack - 1; heightIndex >= 0; heightIndex--) {
            unsigned long newPosition = ((4*stackIndex) + 1) + (4*numStacks*((initialMaxStack-1)-heightIndex));
            fseek(fp, newPosition, SEEK_SET);
            char boxChar = fgetc(fp);
            stacks[stackIndex][heightIndex] = boxChar == ' ' ? '\0' : boxChar;
        }
        printf("Initialized stack: %s\n", stacks[stackIndex]);
    }

    // Reset the file pointer position
    fseek(fp, currentPosition, SEEK_SET);

}


/// @brief Move the file pointer location to the instructions
/// @param fp The file pointer
void moveToInstructions(FILE *fp) {
    char tempReader[MAX_LINE_LENGTH];
    while (true) {
        fgets(tempReader, sizeof(tempReader), fp);
        if (tempReader[0] == '\n') { return; }
    }
}


/// @brief Parse and instruction from a line from the file
/// @param fp The file pointer
/// @return An instruction that should be executed
inst_t parseInstuction(FILE *fp) {
    inst_t inst;
    fscanf(fp, "move %u from %u to %u\n", &inst.num, &inst.src, &inst.dest);
    return inst;
}


/// @brief Execition a given instruction
/// @param stacks The stacks state pointer
/// @param currentInstruction The current instruction to execute
void executeInstruction(char **stacks, inst_t currentInstruction) {

    unsigned long lengthArray = strlen(stacks[currentInstruction.src - 1]);
    char *topBox = &(stacks[currentInstruction.src - 1][lengthArray - currentInstruction.num]);
    printf("***: %s\n", topBox);
    strncat(stacks[currentInstruction.dest - 1], topBox, currentInstruction.num);
    stacks[currentInstruction.src - 1][lengthArray - currentInstruction.num] = '\0';
}


int main(int argc, char **argv) {

    // Define variables for later use
    char fileLine[MAX_LINE_LENGTH];

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Get the number of stacks
    unsigned int numStacks = getNumberOfStacks(fp);
    printf("Number of stacks: %u\n", numStacks);

    // Get the max current stack size
    unsigned int currentMaxStack = getCurrentMaxStack(fp);
    printf("Current max stack size: %u\n", currentMaxStack);

    // Calculate the max stack size need to allocate
    unsigned int maxStackSize = numStacks * currentMaxStack;

    // Allocate memory for each stack
    char *stacks[numStacks];
    for (int index = 0; index < numStacks; index++) {
        stacks[index] = (char *)malloc(maxStackSize + 1);
    }

    // Initialize stacks
    initializeStacks(fp, stacks, numStacks, currentMaxStack);

    // Move to instructions
    moveToInstructions(fp);

    // Iterate through file
    while (true) {

        // Get a line from the file
        inst_t currentInstruction = parseInstuction(fp);

        // Execute the instruction
        executeInstruction(stacks, currentInstruction);

        // Stop iterating if end of file
        if (feof(fp)) { break; }

    }

    // Print the current state of the boxes
    char *topBoxes = malloc(numStacks + 1);
    topBoxes[strlen(topBoxes)] = '\0';
    for (int index = 0; index < numStacks; index++) {
        topBoxes[index] = stacks[index][strlen(stacks[index]) - 1];
    }
    printf("----------------------------\n");
    printf("Top boxes are %s\n", topBoxes);

    // Free the allocated memory
    for (int index = 0; index < numStacks; index++) {
        free(stacks[index]);
    }

}
