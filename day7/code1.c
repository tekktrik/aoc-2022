#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LEN UCHAR_MAX

unsigned long total_count = 0;
unsigned long line_count = 0;

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
    //printf("AAA %sBBB\n", file_line);
    //printf("STRLEN A: %lu\n", strlen(file_line));
    memmove(&file_line[0], &file_line[2], strlen(file_line)-2);
    file_line[strlen(file_line) - 3] = '\0';
    //printf("CCC %sDDD\n", file_line);
    //printf("STRLEN B: %lu\n", strlen(file_line));
}


bool is_command_x(char *file_line, char *command) {
    printf("A: %s\n", file_line);
    printf("len: %lu\n", strlen(file_line));
    printf("B: %s\n", command);
    printf("len: %lu\n", strlen(command));
    char temp_buffer[MAX_LINE_LEN];
    char other_buffer[MAX_LINE_LEN];
    strncpy(temp_buffer, file_line, strlen(file_line));
    //strncpy(other_buffer, command, 2);
    temp_buffer[2] = '\0';
    printf("C: %s\n", temp_buffer);
    printf("len: %lu\n", strlen(temp_buffer));
    bool result = !strcmp(temp_buffer, command);
    printf("%d\n", result);
    return result;
}


command_id_t get_command(char *file_line) {
    printf("IN GETCOMMAND\n");
    if (is_command_x(file_line, "cd")) { return CD; }
    if (is_command_x(file_line, "ls")) { return LS; }
    //printf("IN GET COMMAND with %s\n", file_line);
    printf("Unidentified command reached: %sPQP", file_line);
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


void parse_items(FILE *fp, file_item_t *current_directory) {
    // Something wrong here
    while (true) {
        char next_char = fgetc(fp);
        if (next_char == '$') { break; }
        else if (next_char == 'd') { continue; }
        char temp_buffer[100];
        fseek(fp, -1, SEEK_CUR);
        fgets(temp_buffer, sizeof(temp_buffer), fp);
        line_count++;
        char *str_size = strtok(temp_buffer, " ");
        unsigned long filesize = atoi(str_size);
        current_directory->size += filesize;
    }
}


void parse_file_tree(FILE *fp, file_item_t *current_dir) {

    printf("Now parsing %s\n", current_dir->name);

    // Use variable to track size of directory
    unsigned long long directory_size = 0;

    // Get array to store line in
    char file_line[MAX_LINE_LEN];

    // Iterate through file
    while (true) {

        // Read a line from the file
        fgets(file_line, MAX_LINE_LEN, fp);
        line_count++;
        total_count++;
        printf("Total dirs seen: %lu\n", total_count);

        printf("@ PARSING LINE: %s", file_line);

        // Break if end of file reached
        if (feof(fp)) { break; }  // TODO: Change to return stucture

        printf("Before striping: %sPPP\n", file_line);
        if (is_command(file_line)) {  // If command...
            printf("Still checking line: %s", file_line);
            strip_command_flag(file_line);
            //printf("After striping: %sZZZ\n", file_line);
            char *new_dir_name;
            switch (get_command(file_line)) {
                case CD:
                    /*
                    printf("Parsing cd...\n");
                    new_dir_name = parse_cd_command(file_line);
                    printf("Parsing new directory %s\n", new_dir_name);
                    //printf("Parsed new directory name!\n");
                    if (!strcmp(new_dir_name, "..")) {  // Travel up one directory
                        // TODO: Add size
                        printf("Traveling up!\n");
                        //printf("This directory's size: %lu\n", current_dir->size);
                        return;
                    }
                    else {  // Enter new directory
                        // TODO: Create new file item
                        file_item_t **contained_items = malloc(sizeof(file_item_t));
                        file_item_t *child_dir = malloc(sizeof(file_item_t));
                        //printf("Checkpoint A\n");
                        child_dir->file_type = DIR_ID;
                        child_dir->size = 0;
                        child_dir->num_items = 0;
                        child_dir->name = new_dir_name;  // TODO: Use new directory name
                        child_dir->items = (void **)contained_items;
                        current_dir->num_items = current_dir->num_items + 1;
                        //printf("Checkpoint B\n");
                        //printf("New size: %lu\n", (current_dir->num_items)*sizeof(file_item_t));
                        //printf("pre: %lu\n", current_dir->items);
                        file_item_t **temp_ptr = (file_item_t **)realloc(current_dir->items, (current_dir->num_items)*sizeof(file_item_t));
                        //printf("post %lu\n", temp_ptr);
                        if (temp_ptr == NULL) { printf("Failed realloc!\n"); exit(1); }
                        else { current_dir->items = (void **)temp_ptr; }
                        //printf("Checkpoint C\n");
                        ((file_item_t **)(current_dir->items))[current_dir->num_items - 1] = child_dir;
                        printf("TO PARSE: %s\n", child_dir->name);
                        parse_file_tree(fp, child_dir);
                        printf("DONE PARSING: %s\n", child_dir->name);
                        current_dir->size += child_dir->size;
                    }
                    */
                    break;
                case LS:
                    printf("Parsing ls...\n");
                    // TODO: Parse files listed by ls command
                    // NOTE: DIR commands can probably be ignored!
                    parse_items(fp, current_dir);
                    break;
                default:
                    printf("Unidentified command reached, exiting.");
                    exit(1);
            }
        }
        else { // Not command..
            printf("HELLO?\n");
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
    char *base_name = malloc(2);
    char *temp_name = "/";
    memmove(base_name, temp_name, 2);

    // Initialize list of directories
    file_item_t **contained_items = malloc(1);

    // Initialize current directory
    file_item_t current_directory = { DIR_ID, 0, 0, base_name, (void **)contained_items };

    // Get rid of first line since it's just setting to base directory
    char trash_buffer[10];
    fgets(trash_buffer, 10, fp);
    printf("trash: %sQQQ", trash_buffer);

    // Begin parsing stucture
    parse_file_tree(fp, &current_directory);
    //printf("Size of %lu\n", sizeof(file_item_t));

    // Return exit code for success
    printf("Lines counted: %lu\n", line_count);
    printf("EXIT\n");
    return 0;

}
