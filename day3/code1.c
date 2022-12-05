#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// Definitions for maximum rucksack and compartment sizes
#define RUCKSACK_MAX 50
#define RUCKSACK_HALF RUCKSACK_MAX/2

// Define global variables
FILE *fp;
char rucksack[RUCKSACK_MAX];


/// @brief Get a unique set of items for a compartment
/// @param setPointer A pointer to the place where the unique set will be stored
/// @param compartment The character array to parse for unique items
void getItemSet(char *setPointer, char compartment[]) {
    unsigned int setIndex = 0;
    for (int index = 0; index < strlen(compartment); index++) {
        if (compartment[index] == '\0') {
            break;
        }
        bool newChar = true;
        if (index != 0) {
            for (int checkIndex = 0; checkIndex < strlen(setPointer); checkIndex++) {
                if (setPointer[checkIndex] == compartment[index]) {
                    newChar = false;
                    break;
                }
            }
        }
        if (newChar) {
            setPointer[setIndex] = compartment[index];
            setPointer[setIndex + 1] = '\0';
            setIndex += 1;
        }
    }
}


/// @brief Get the shared character for two unique sets of characters
/// @param firstCompartment The first unique character array
/// @param secondCompartment The second unique character array
/// @return The character shared between the two character arrays
char getSharedChar(char *firstCompartment, char *secondCompartment) {
    unsigned int firstLen = strlen(firstCompartment);
    unsigned int secondLen = strlen(secondCompartment);

    for (int firstIndex = 0; firstIndex < firstLen; firstIndex++) {
        for (int secondIndex = 0; secondIndex < secondLen; secondIndex++) {
            if (firstCompartment[firstIndex] == secondCompartment[secondIndex]) {
                return firstCompartment[firstIndex];
            }
        }
    }
}


/// @brief Score the unique item
/// @param uniqueItem The unique item, given as a character
/// @return The score of the item
unsigned int scoreItem(char uniqueItem) {
    if (uniqueItem >= 'a' && uniqueItem <= 'z') {
        return (uniqueItem - 'a') + 1;
    }
    else if (uniqueItem >= 'A' && uniqueItem <= 'Z') {
        return (uniqueItem - 'A') + 27;
    }
}


int main(int argc, char *argv[]) {

    unsigned int totalScore = 0;
    
    // Open the file in read mode
    fp = fopen(argv[1], "r");

    // Loop through reading lines from the input file
    while (true) {

        // Read a line from the input file
        fgets(rucksack, sizeof(rucksack), fp);

        // Remove trailing newline character from line
        rucksack[strcspn(rucksack, "\n")] = 0;

        // Break if detected empty newline
        if (feof(fp)) { break; }

        // Get a the dimensions of the rucksack and compartments
        unsigned int rucksackSize = strlen(rucksack);
        unsigned long compartmentSize = rucksackSize/2;

        // Allocate memory for the rucksack compartments
        char *compartA = malloc(compartmentSize + 1);
        char *compartB = malloc(compartmentSize + 1);

        // Split the rucksack inventory into the two compartments
        strncpy(compartA, rucksack, compartmentSize);
        compartA[compartmentSize] = '\0';
        strncpy(compartB, rucksack+compartmentSize, compartmentSize);
        compartB[compartmentSize] = '\0';

        // Allocate memory for a unique set of items for each compartment
        char *uniqueA = malloc(strlen(compartA));
        char *uniqueB = malloc(strlen(compartB));

        // Get a unique list of items for each compartment
        getItemSet(uniqueA, compartA);
        getItemSet(uniqueB, compartB);

        // Get the shared character and add to the total score
        char sharedChar = getSharedChar(uniqueA, uniqueB);
        totalScore += scoreItem(sharedChar);

        // Free the compartment memory
        free(compartA);
        free(compartB);
        free(uniqueA);
        free(uniqueB);

    }

    // Print result
    printf("Total score: %u\n", totalScore);

    // Return exit code 0 for success
    return 0;
}
