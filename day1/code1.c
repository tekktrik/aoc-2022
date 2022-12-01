#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void main(void) {

    // Decclare variables
    FILE *fp;
    unsigned long itemCal;
    char textCal[50];
    char *endPtr;
    unsigned long maxElfTotal = 0;
    
    // Open the file
    fp = fopen("./foodlist1.txt", "r");

    // Read file line by line
    while (!feof(fp)) {

        // Store the current elf's total
        static unsigned long currentElfTotal = 0;

        // Get the current line
        fgets(textCal, 50, fp);

        // Check for a blank line to close out elf's total
        if (textCal[0] == '\n') {

            // If the current elf's total is great than current
            // max, save it
            if (currentElfTotal > maxElfTotal) {
                maxElfTotal = currentElfTotal;
                printf("New max total: %lu\n", maxElfTotal);
            }

            // Reset the current elf's total and continue with loop
            currentElfTotal = 0;
            continue;
        }

        // Parse the number and add it to the current elf's total
        itemCal = strtoul(textCal, &endPtr, 10);
        currentElfTotal += itemCal;
    }
}
