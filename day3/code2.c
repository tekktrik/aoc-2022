#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// Definitions for maximum rucksack and compartment sizes
#define RUCKSACK_MAX 50
#define RUCKSACK_HALF RUCKSACK_MAX/2
#define ELF_GROUP_SIZE 3

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


/// @brief Get the shared character between the elves
/// @param firstRuck The first elf's unique set of rucksack items
/// @param secondRuck The second elf's unique set of rucksack items
/// @param thirdRuck The third elf's unique set of rucksack items
/// @return The unique item shared between all the elves
char getSharedChar(char *firstRuck, char *secondRuck, char *thirdRuck) {
    unsigned int firstLen = strlen(firstRuck);
    unsigned int secondLen = strlen(secondRuck);
    unsigned int thirdLen = strlen(thirdRuck);

    for (int firstIndex = 0; firstIndex < firstLen; firstIndex++) {
        for (int secondIndex = 0; secondIndex < secondLen; secondIndex++) {
            for (int thirdIndex = 0; thirdIndex < thirdLen; thirdIndex++) {
                if (firstRuck[firstIndex] == secondRuck[secondIndex] && firstRuck[firstIndex] == thirdRuck[thirdIndex]) {
                    return firstRuck[firstIndex];
                }
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

        // Allocate memory for the rucksack compartments
        char *elfA = malloc(sizeof(rucksack));
        char *elfB = malloc(sizeof(rucksack));
        char *elfC = malloc(sizeof(rucksack));

        // Read a line from the input file
        fgets(elfA, sizeof(rucksack), fp);
        fgets(elfB, sizeof(rucksack), fp);
        fgets(elfC, sizeof(rucksack), fp);

        // Remove trailing newline character from line
        elfA[strcspn(elfA, "\n")] = 0;
        elfB[strcspn(elfB, "\n")] = 0;
        elfC[strcspn(elfC, "\n")] = 0;

        // Break if detected empty newline
        if (feof(fp)) { break; }

        // Allocate memory for a unique set of items for each compartment
        char *uniqueA = malloc(strlen(elfA));
        char *uniqueB = malloc(strlen(elfB));
        char *uniqueC = malloc(strlen(elfC));

        // Get a unique list of items for each compartment
        getItemSet(uniqueA, elfA);
        getItemSet(uniqueB, elfB);
        getItemSet(uniqueC, elfC);

        // Get the shared character and add to the total score
        char sharedChar = getSharedChar(uniqueA, uniqueB, uniqueC);
        totalScore += scoreItem(sharedChar);

        // Free the compartment memory
        free(elfA);
        free(elfB);
        free(elfC);
        free(uniqueA);
        free(uniqueB);
        free(uniqueC);

    }

    // Print result
    printf("Total score: %u\n", totalScore);

    // Return exit code 0 for success
    return 0;
}
