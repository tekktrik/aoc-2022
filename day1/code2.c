#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Define the total number of max calories to store
#define NUM_MAX_TOTALS 3

// Decclare variables
FILE *fp;
unsigned long itemCal;
char textCal[50];
char *endPtr;
long maxElfTotals[3] = {-1, -1, -1};

/// @brief Shift calorie totals left and add a new entry in the specified index
/// @param numList A list of calorie totals
/// @param listLength The length of the provided list
/// @param shiftIndex The index to free
/// @param newEntry The new calorie total entry to add
void shiftArrayAndAdd(long *numList, unsigned long listLength, unsigned long shiftIndex, unsigned long newEntry) {
    for (unsigned long index = 1; index <= shiftIndex; index++) {
        numList[index - 1] = numList[index];
    }
    numList[shiftIndex] = newEntry;
}

/// @brief Get the index to place a new calorie total
/// @param numList A list of calorie totals
/// @param listLength The length of the provided list
/// @param newEntry The new calorie total to add
/// @return The index to insert the new calorie total
unsigned long getIndexPlacement(long *numList, unsigned long listLength, unsigned long newEntry) {
    for (unsigned long index = 1; index < listLength; index++) {
        if (newEntry > numList[index - 1] && newEntry <= numList[index] ) {
            return index - 1;
        }
    }
    return listLength - 1;
}

/// @brief Get the index of an empty slot in the array
/// @param numList A list of calorie totals
/// @param listLength The length of the provided list
/// @return The index of the next empty space, or -1 if none is available
int checkEmptySlot(long *numList, unsigned long listLength) {
    for (unsigned int index = 0; index < listLength; index++) {
        if (numList[index] == -1) {
            return index;
        }
    }
    return -1;
}

/// @brief Sort an array in ascending order
/// @param numList A list of calorie totals
/// @param listLength The length of the provided list
void sortArray(long *numList, unsigned long listLength) {
    int currLength = checkEmptySlot(numList, listLength);

    currLength = currLength == -1 ? NUM_MAX_TOTALS - 1 : currLength - 1;

    while (currLength > 0) {
        if (numList[currLength] < numList[currLength - 1]) {
            long tempStorage = numList[currLength];
            numList[currLength] = numList[currLength - 1];
            numList[currLength - 1] = tempStorage;
        }
        currLength -= 1;
    }
}

void main(void) {
    
    // Open the file
    fp = fopen("./foodlist1.txt", "r");

    // Read file line by line
    while (!feof(fp)) {

        // Store the current elf's total
        static long currentElfTotal = 0;

        // Get the current line
        fgets(textCal, 50, fp);

        // Check for a blank line to close out elf's total
        if (textCal[0] == '\n') {

            // Check for empty slots in the max array and add if possible
            int emptyIndex = checkEmptySlot(maxElfTotals, NUM_MAX_TOTALS);
            if (emptyIndex != -1) {
                maxElfTotals[emptyIndex] = currentElfTotal;
                sortArray(maxElfTotals, NUM_MAX_TOTALS);
            }

            // If the current elf's total is great than current max, save it
            else if (currentElfTotal > maxElfTotals[0]) {
                unsigned long newIndex = getIndexPlacement(maxElfTotals, NUM_MAX_TOTALS, currentElfTotal);
                shiftArrayAndAdd(maxElfTotals, NUM_MAX_TOTALS, newIndex, currentElfTotal);
            }

            // Reset the current elf's total and continue with loop
            currentElfTotal = 0;
            continue;
        }

        // Parse the number and add it to the current elf's total
        itemCal = strtoul(textCal, &endPtr, 10);
        currentElfTotal += itemCal;
    }

    // Add up all the store max calorie totals
    unsigned long interElfTotal = 0;
    for (unsigned long index = 0; index < NUM_MAX_TOTALS; index++) {
        interElfTotal += maxElfTotals[index];
    }

    // Print the result
    printf("Total between top %i elves: %li\n", NUM_MAX_TOTALS, interElfTotal);
}
