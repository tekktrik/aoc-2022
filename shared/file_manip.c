#include <stdio.h>
#include <stdbool.h>

#include "custom_limits.h"
#include "file_manip.h"

/// @brief Get the number of rows on the elevation map
/// @param fp A pointer to the open file
/// @return The number of rows
unsigned int get_num_rows(FILE *fp) {

    // Get a buffer to store characters
    char file_line[MAX_LINE_LEN];

    // Get current file pointer position and set to beginning
    unsigned long current_position = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Count the number of rows
    unsigned int num_rows = 0;
    while (true) {
        fgets(file_line, sizeof(file_line), fp);
        if (feof(fp)) { break; }
        num_rows++;
    }

    // Reset the file pointer position
    fseek(fp, current_position, SEEK_SET);

    // Return the number of rows
    return num_rows;

}