#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Defintion for max file line length
#define MAX_LINE_LEN 25

// Declare global variables
FILE *fp;
char fileLine[MAX_LINE_LEN];
unsigned long subsetCount = 0;

// Type definitions for an elf's duties
typedef struct elfDuties {
    int start;
    int end;
} elf_set_t;


/// @brief Check if an elf's duties  overlap the other's duties
/// @param elfASet The duties of elf A
/// @param elfBSet The duties of elf B
/// @return Whether the one elf's duties overlaps the other's duties
bool hasOverlap(elf_set_t elfASet, elf_set_t elfBSet) {
    if (elfBSet.start >= elfASet.start && elfBSet.start <= elfASet.end) { return true; }
    if (elfBSet.end >= elfASet.start && elfBSet.end <= elfASet.end) { return true; }
    if (elfASet.start >= elfBSet.start && elfASet.start <= elfBSet.end) { return true; }
    if (elfASet.end >= elfBSet.start && elfASet.end <= elfBSet.end) { return true; }
    return false;
}


int main(int argc, char *argv[]) {

    // Open the file provided as an argument
    fp = fopen(argv[1], "r");

    // Loop through file, reading line by line
    while (true) {

        // Read a line from the input file
        fgets(fileLine, MAX_LINE_LEN, fp);

        // Break if end of file reached
        if (feof(fp)) { break; }

        // Split the line into elves A and B
        char *elfA = strtok(fileLine, ",");
        char *elfB = strtok(NULL, "\n");

        // Get starts and ends from elves' data
        int startA = atoi(strtok(elfA, "-"));
        int endA = atoi(strtok(NULL, ""));
        int startB = atoi(strtok(elfB, "-"));
        int endB = atoi(strtok(NULL, ""));

        // Create structs from information
        elf_set_t elfSetA = { startA, endA };
        elf_set_t elfSetB = { startB, endB };

        // Add to tally if subset
        if (hasOverlap(elfSetA, elfSetB)) { subsetCount += 1; }

    }

    // Print results
    printf("Total number of subsets: %lu\n", subsetCount);

    // Return exit code zero for success
    return 0;

}
