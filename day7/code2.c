#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

// Defintions
#define MAX_LINE_LEN UCHAR_MAX

// Global variable for holding total applicable folder size
unsigned long long total_folder_sizes = 0;

// Define type definition for commands
typedef enum { CD, LS } command_id_t;
typedef enum { FILE_ID, DIR_ID } file_item_id_t;

// Type definition for working with file items
typedef struct FileItem {
    file_item_id_t file_type;  // File type ID
    unsigned long size;  // Current size of file item
    unsigned long num_items;  // Number of file items
    char *name;  // Name of file item
    void **items;  // List of file items
} file_item_t;


/// @brief Whether a file line is a command
/// @param file_line The file line to parse
/// @return Whether the given file line was a command
bool is_command(char *file_line) {
    return file_line[0] == '$';
}


/// @brief Strip the command flag from the line
/// @param file_line The line to parse and strip
void strip_command_flag(char *file_line) {
    memmove(&file_line[0], &file_line[2], strlen(file_line)-2);
    file_line[strlen(file_line) - 3] = '\0';
}


/// @brief Check whether a specific command is being issued
/// @param file_line The line to parse
/// @param command The command to check for
/// @return Whether the given command was the one issued in the line
bool is_command_x(char *file_line, char *command) {
    char temp_buffer[MAX_LINE_LEN];
    char other_buffer[MAX_LINE_LEN];
    strncpy(temp_buffer, file_line, strlen(file_line));
    temp_buffer[2] = '\0';
    bool result = !strcmp(temp_buffer, command);
    return result;
}


/// @brief Get the command for the current line
/// @param file_line The line to parse
/// @return The specific command being used
command_id_t get_command(char *file_line) {
    if (is_command_x(file_line, "cd")) { return CD; }
    if (is_command_x(file_line, "ls")) { return LS; }
    printf("Unidentified command reached: %sPQP", file_line);
    printf("Exiting!\n");
    exit(1);
}


/// @brief Parse the cd command
/// @param file_line A pointer the line to parse
/// @return The directory name used as an argument to cd
char * parse_cd_command(char *file_line) {
    char *cd_command = strtok(file_line, " ");
    char *directory = strtok(NULL, "\n");
    char *ret_directory = malloc(strlen(directory));
    memmove(ret_directory, directory, strlen(directory));
    ret_directory[strlen(directory)] = '\0';
    return ret_directory;
}


/// @brief Parse items returned by the ls command
/// @param fp A pointer to the open file
/// @param current_directory The cirrent directory being parsed
void parse_items(FILE *fp, file_item_t *current_directory) {
    
    // Allocate buffer for reading through file without parsing
    char trash_buffer[MAX_LINE_LEN];

    // Iterate through file
    while (true) {

        // Get the next character
        char next_char = fgetc(fp);
        if (next_char == '$') {  // If next character is part of command
            fseek(fp, -1L, SEEK_CUR);
            return;
        }
        else if (next_char == 'd') {  // If next character is directory name
            fgets(trash_buffer, sizeof(trash_buffer), fp);
            continue;
        }
        else if (next_char - 48 > 9 || next_char - 48 < 0) { return; }  // Error if next character is otherwise not a number
        
        // Rewind file location by one to account for character
        fseek(fp, -1L, SEEK_CUR);

        // Allocate memory and parse the line
        char temp_buffer[100];
        fgets(temp_buffer, sizeof(temp_buffer), fp);

        // Get the file size
        char *str_size = strtok(temp_buffer, " ");
        unsigned long filesize = atoi(str_size);

        // Add file size to the parent directory
        current_directory->size += filesize;

        // Return if end of file reached
        if (feof(fp)) { return; }
    }
}


/// @brief Parse a directory
/// @param fp A pointer to the open file
/// @param current_dir The directory to parse
void parse_file_tree(FILE *fp, file_item_t *current_dir) {

    // Use variable to track size of directory
    unsigned long long directory_size = 0;

    // Get array to store line in
    char file_line[MAX_LINE_LEN];

    // Iterate through file
    while (true) {

        // Read a line from the file
        fgets(file_line, MAX_LINE_LEN, fp);

        // Break if end of file reached
        if (feof(fp)) { break; }  // TODO: Change to return stucture

        // Check if command was issued
        if (is_command(file_line)) {  // If command...

            // Stip the command flag from the line
            strip_command_flag(file_line);

            // Check which command was issued
            switch (get_command(file_line)) {
                case CD:  // cd command

                    // Parse the cd command for the directory name
                    char *new_dir_name = parse_cd_command(file_line);

                    // Check whether the command was "cd .." or "cd [directory]"
                    if (!strcmp(new_dir_name, "..")) {  // Travel up one directory

                        // Free the memory of the string
                        free(new_dir_name);

                        // Add the new directory to the total
                        total_folder_sizes += current_dir->size;

                        // Return to parent directory to continue parsing
                        return;

                    }
                    else {  // Enter new directory

                        // Allocate memory for new child directory and it's own children
                        file_item_t **contained_items = malloc(sizeof(file_item_t));
                        file_item_t *child_dir = malloc(sizeof(file_item_t));

                        // Initialize the child directory
                        child_dir->file_type = DIR_ID;
                        child_dir->size = 0;
                        child_dir->num_items = 0;
                        child_dir->name = new_dir_name;  // TODO: Use new directory name
                        child_dir->items = (void **)contained_items;

                        // Reallocate memory for the current directory's children to add this one
                        current_dir->num_items = current_dir->num_items + 1;
                        file_item_t **temp_ptr = (file_item_t **)realloc(current_dir->items, (current_dir->num_items)*sizeof(file_item_t));

                        // Add pointer to children, or fail if memory cannot be allocated
                        if (temp_ptr == NULL) { printf("Failed realloc!\n"); exit(1); }
                        else { current_dir->items = (void **)temp_ptr; }
                        ((file_item_t **)(current_dir->items))[current_dir->num_items - 1] = child_dir;

                        // Parse the file tree for the child directory
                        parse_file_tree(fp, child_dir);

                        // Add the child directory's size to the current one's size
                        current_dir->size += child_dir->size;
                    }
                    break;
                case LS:
                    // Parse the items returned by the ls command
                    parse_items(fp, current_dir);
                    break;
                default:
                    // Something went wrong, this shouldn't be possible
                    printf("Unidentified command reached, exiting.");
                    exit(1);
            }
        }
        else { // Not command..
            printf("Error!");
            exit(1);
        }
    }
}


/// @brief Check whether to update the best deletion candidate size
/// @param candidate_size Current candidate size
/// @param needed_size The size needed to be freed
/// @param current_max The current best deletion size
/// @return The current best deletion size between the two candidates
unsigned long long check_for_new_deletion(unsigned long long candidate_size, unsigned long long needed_size, unsigned long long current_max) {
    if (candidate_size < current_max && candidate_size >= needed_size) {
        current_max = candidate_size;
    }
    return current_max;
}


/// @brief Get the folder deletion size
/// @param current_directory The current directory to check
/// @param needed_size The amount of space needed to be freed
/// @param deletion_candidate_size The current best deletion size
/// @return The best deletion size for this directory and its children
unsigned long long select_deletion_folder_size(file_item_t *current_directory, unsigned long long needed_size, unsigned long long deletion_candidate_size) {
    unsigned long long current_max_value = deletion_candidate_size;
    current_max_value = check_for_new_deletion(current_directory->size, needed_size, current_max_value);
    for (int child_index = 0; child_index < current_directory->num_items; child_index++) {
        file_item_t *child = ((file_item_t **)current_directory->items)[child_index];
        current_max_value = select_deletion_folder_size(child, needed_size, current_max_value);
    }
    return current_max_value;

}


int main(int argc, char **argv) {

    // Open input file
    FILE *fp = fopen(argv[1], "r");

    // Initialize base name
    char *base_name = malloc(2);
    char *temp_name = "/";
    memmove(base_name, temp_name, 2);

    // Initialize list of directories
    file_item_t **contained_items = malloc(sizeof(file_item_t));

    // Initialize current directory
    file_item_t current_directory = { DIR_ID, 0, 0, base_name, (void **)contained_items };

    // Get rid of first line since it's just setting to base directory
    char trash_buffer[10];
    fgets(trash_buffer, 10, fp);

    // Begin parsing stucture
    parse_file_tree(fp, &current_directory);

    // Calculate space needed
    unsigned long long device_size = atoll(argv[2]);
    unsigned long long home_size = current_directory.size;
    unsigned long long unused_size = device_size - home_size;
    unsigned long long update_size = atoll(argv[3]);
    unsigned long long needed_size = update_size - unused_size;

    // Select the folder to delete
    unsigned long long deletion_size = home_size;
    deletion_size = select_deletion_folder_size(&current_directory, needed_size, deletion_size);

    // Return exit code for success
    printf("Folder deletion size: %llu\n", deletion_size);
    return 0;

}