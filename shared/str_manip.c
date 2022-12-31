#include <string.h>


/// @brief Trim characters on the left side of a string
/// @param file_line The string
/// @param n The number of characters to trim
void ltrim_chars(char *file_line, unsigned int n) {
    char *new_line = file_line + n;
    memcpy(file_line, new_line, strlen(new_line));
    file_line[strlen(new_line)] = '\0';
}
