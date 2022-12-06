#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_LINE_LENGTH UCHAR_MAX


typedef struct StackInfo {
    unsigned int index;
    unsigned int size;
    char *stackOrder;
} stack_info_t;


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


void allocateStacks(stack_info_t *stacks, unsigned int stackCount, unsigned int initialMaxStack) {

    // Get the max size needed for each stack array
    unsigned int maxStackSize = (stackCount * initialMaxStack) + 1;
    unsigned int structSize = sizeof(unsigned int)*2 + sizeof(char)*maxStackSize;

    // Allocate an array for the stacks list
    stacks = malloc(stackCount * structSize);

    // Add pointers to each character array
    for (int index = 0; index < stackCount; index++) {
        printf("111FDSHFUD2233S\n");
        //stacks[index]->index = index;
        //stacks[index]->size = 5;
        stacks[index]->stackOrder = "ABCE";
        //stacks[index] = {index, 5, "ABCE"};
        printf("IFIFIF: %s\n", stacks[index]->stackOrder);
    }

    printf("FDSHFUD2233S\n");

}


void deallocateStacks(stack_info_t **stacks, unsigned int numStacks) {
    free(stacks);
}


void initializeStacks(FILE *fp, stack_info_t **stacks, unsigned int numStacks, unsigned int initialMaxStack) {
    
    // Get current file pointer position and set to beginning
    unsigned long currentPosition = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    for (int stackIndex = 0; stackIndex < numStacks; stackIndex++) {
        printf("FDSD2S: %i\n", stackIndex);
        printf("SSS:\n");
        printf("OHH: %s\n", stacks[stackIndex]->stackOrder);
        stacks[stackIndex]->stackOrder[initialMaxStack] = '\0';
        printf("QQQ\n");
        for (int heightIndex = initialMaxStack - 1; heightIndex >= 0; heightIndex--) {
            printf("AAA\n");
            unsigned long newPosition = ((4*stackIndex) + 1) + (4*numStacks*((initialMaxStack-1)-heightIndex));
            fseek(fp, newPosition, SEEK_SET);
            char boxChar = fgetc(fp);
            stacks[stackIndex]->stackOrder[heightIndex] = boxChar == ' ' ? '\0' : boxChar;
        }
        printf("Initial stack: %s\n", stacks[stackIndex]->stackOrder);
    }

    // Reset the file pointer position
    fseek(fp, currentPosition, SEEK_SET);

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

    // Allocate memory for stacks
    stack_info_t **stacksInfo;
    allocateStacks(stacksInfo, numStacks, currentMaxStack);
    for (int tempy = 0; tempy < numStacks; tempy++) {
        printf("This is %s\n", stacksInfo[tempy]->stackOrder);
    }

    // Initialize stacks
    initializeStacks(fp, stacksInfo, numStacks, currentMaxStack);

    // Iterate through file
    while (true) {

        // Get a line from the file
        fgets(fileLine, sizeof(fileLine), fp);

        // Stop iterating if end of file
        if (feof(fp)) { break; }

        // Remove the newline character from the string
        fileLine[strcspn(fileLine, "\n")] = 0;

    }

    // Deallocate the memory used in the heap
    deallocateStacks(stacksInfo, numStacks);

}
