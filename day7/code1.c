#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LEN UCHAR_MAX

unsigned long total_count = 0;

// Define type definition for commands
typedef enum { CD, LS } command_id_t;
typedef enum { FILE_ID, DIR_ID } file_item_id_t;


typedef struct FileItem {
    file_item_id_t file_type;  // File type ID
    unsigned long size;  // Current size of file item
    unsigned long num_items;  // Number of file items
    char *name;  // Name of file item
    void **items;  // List of file items
} file_item_t;


bool is_command(char *file_line) {
    return file_line[0] == '$';
}


void strip_command_flag(char *file_line) {
    memmove(&file_line[0], &file_line[2], strlen(file_line) - 2);
}


bool is_command_x(char *file_line, char *command) {
    char *temp_buffer = malloc(strlen(command) + 1);
    temp_buffer[strlen(command)] = '\0';
    strncpy(temp_buffer, file_line, strlen(command));
    int result = !strcmp(temp_buffer, command);
    free(temp_buffer);
    return result;
}


command_id_t get_command(char *file_line) {
    if (is_command_x(file_line, "cd")) { return CD; }
    if (is_command_x(file_line, "ls")) { return LS; }
    printf("Unidentified command reached: %s", file_line);
    printf("Exiting!\n");
    exit(1);
}


bool is_dir(char *file_line) {
    char temp_buffer[4];
    temp_buffer[3] = '\0';
    strncpy(temp_buffer, file_line, 3);
    bool result = !strcmp(temp_buffer, "dir");
    return result;
}


char * parse_cd_command(char *file_line) {
    char *cd_command = strtok(file_line, " ");
    char *directory = strtok(NULL, "\n");
    printf("directory: %s\n", directory);
    char *ret_directory = malloc(strlen(directory));
    memmove(ret_directory, directory, strlen(directory));
    ret_directory[strlen(directory)] = '\0';
    return ret_directory;
}


void parse_file_tree(FILE *fp, file_item_t *current_dir) {

    // Use variable to track size of directory
    unsigned long long directory_size = 0;

    // Get array to store line in
    char file_line[MAX_LINE_LEN];

    // Iterate through file
    while (true) {

        // Read a line from the file
        fgets(file_line, MAX_LINE_LEN, fp);
        total_count++;
        printf("Total dirs seen: %lu\n", total_count);

        // Break if end of file reached
        if (feof(fp)) { break; }  // TODO: Change to return stucture

        if (is_command(file_line)) {  // If command...
            strip_command_flag(file_line);
            /*
            switch (get_command(file_line)) {
                case CD:
                    printf("");
                    //printf("Parsing cd...\n");
                    char *new_dir_name;
                    new_dir_name = parse_cd_command(file_line);
                    //printf("Parsed new directory name!\n");
                    if (!strcmp(new_dir_name, "..")) {  // Travel up one directory
                        // TODO: Add size
                        //printf("Traveling up!\n");
                    }
                    else {  // Enter new directory
                        //printf("Parsing new directory %s\n", new_dir_name);
                        // TODO: Create new file item
                        file_item_t **contained_items = malloc(0);
                        file_item_t *child_dir = malloc(sizeof(file_item_t));
                        //printf("Checkpoint A\n");
                        child_dir->file_type = DIR_ID;
                        child_dir->size = 0;
                        child_dir->num_items = 0;
                        child_dir->name = new_dir_name;  // TODO: Use new directory name
                        child_dir->items = (void **)contained_items;
                        current_dir->num_items = current_dir->num_items + 1;
                        //printf("Checkpoint B\n");
                        printf("New size: %lu\n", (current_dir->num_items)*sizeof(file_item_t));
                        printf("pre: %lu\n", current_dir->items);
                        file_item_t **temp_ptr = (file_item_t **)realloc(current_dir->items, 10000*(current_dir->num_items)*sizeof(file_item_t));
                        printf("post %lu\n", temp_ptr);
                        if (temp_ptr == NULL) { printf("Failed realloc!\n"); exit(1); }
                        else { current_dir->items = (void **)temp_ptr; }
                        //printf("Checkpoint C\n");
                        ((file_item_t **)(current_dir->items))[current_dir->num_items - 1] = child_dir;
                        //printf("Parsed child directory!\n");
                        // TODO: Add child directory to parent
                        // TODO: Parse the new directory
                        // TODO: Return out of new directory and add size
                    }
                    //printf("OTHER 1\n");
                    break;
                case LS:
                    //printf("Parsing ls...\n");
                    // TODO: Parse files listed by ls command
                    // NOTE: DIR commands can probably be ignored!
                    break;
                default:
                    //printf("Unidentified command reached, exiting.");
                    exit(1);
                    break;
            }
            */
        }
        else { // Not command..
            if (is_dir(file_line)) {  // Is directory...
                //printf("pass a\n");
            }
            else { // Is file...
                //printf("pass b\n");
            }
        }
        //printf("Did complete X!\n");
    }
    //printf("Did complete Y!\n");
}


int main(int argc, char **argv) {

    // Open input file
    FILE *fp = fopen(argv[1], "r");

    // Initialize base name
    char *base_name = malloc(0);
    char *temp_name = "/";
    memmove(base_name, temp_name, 1);

    // Initialize list of directories
    file_item_t **contained_items = malloc(1);

    // Initialize current directory
    file_item_t current_directory = { DIR_ID, 0, 0, base_name, (void **)contained_items };

    // Get rid of first line since it's just setting to base directory

    // Begin parsing stucture
    parse_file_tree(fp, &current_directory);
    //printf("Size of %lu\n", sizeof(file_item_t));

    // Return exit code for success
    printf("EXIT\n");
    return 0;

}
