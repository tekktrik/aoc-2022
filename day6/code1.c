#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/// @brief Get the next character in the file
/// @param fp A pointer to the file
/// @param position A pointer to the current string position variable
/// @return The next character in the file
char getNextChar(FILE *fp, unsigned long *position) {
    char nextChar = fgetc(fp);
    (*position)++;
    return nextChar;
}


/// @brief Shift the buffer and add the new element
/// @param buffer The buffer to shift and add to
/// @param newChar The new character to add
void shiftAndAdd(char *buffer, char newChar) {
    unsigned int strLen = strlen(buffer);
    for (int index = 0; index < strLen - 1; index++) {
        buffer[index] = buffer[index + 1];
    }
    buffer[strLen - 1] = newChar;
}


/// @brief Whether the current buffer has any repeat characters
/// @param buffer The current character buffer
/// @param numChars The number of characters in the character buffer
/// @return Whether there are any repeats
bool hasRepeats(char *buffer, unsigned int numChars) {

    // Assign temporary variables for storing unique characters
    char *tempBuffer = malloc(numChars + 1);
    tempBuffer[0] = '\0';
    unsigned int tempIndex = 0;

    // Iterate through buffer and create a buffer of unique characters
    for (int index = 0; index <= numChars; index++) {
        bool newChar = true;
        if (index != 0) {
            //printf("Initial add\n");
            for (int bufferIndex = 0; bufferIndex < strlen(tempBuffer); bufferIndex++) {
                if (buffer[index] == tempBuffer[bufferIndex]) {
                    newChar = false;
                    break;
                }
            }
        }
        if (newChar) {
            strncat(tempBuffer, &buffer[index], 1);
            tempIndex++;
        }
    }

    // Free any allocated memory
    free(tempBuffer);

    // Return whether the arrays have different lengths
    return (strlen(buffer) != strlen(tempBuffer));
}


int main(int argc, char **argv) {

    // Open the input file
    FILE *fp = fopen(argv[1], "r");
    unsigned int numChars = atoi(argv[2]);
    unsigned long position = numChars;

    // Get the first three characters
    char *buffer = malloc(numChars + 1);
    buffer[numChars] = '\0';
    fgets(buffer, numChars + 1, fp);

    // Iterate through the file
    while (!feof(fp) && hasRepeats(buffer, numChars)) {
        
        // Get the next character in the stream
        char nextChar = getNextChar(fp, &position);

        // Report error if newline character reached
        if (nextChar == '\n') {
            printf("No unique code found!");
            return 1;
        }

        shiftAndAdd(buffer, nextChar);

    }

    // Print out the unique string and index
    printf("Unique code: %s\n", buffer);
    printf("Position: %lu\n", position);

    // Free the allocated memory
    free(buffer);

    // Return exit code 0 for success
    return 0;

}
