#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


#define MAX_LINE_LEN UCHAR_MAX


typedef struct GridTree {
    int grid_value;
    char height;
} tree_t;


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

        num_rows++; }

    // Reset the file pointer position
    fseek(fp, current_position, SEEK_SET);

    // Return the number of rows
    return num_rows;

}

unsigned int get_num_cols(FILE *fp) {

    // Get a buffer to store characters
    char file_line[MAX_LINE_LEN];

    // Get current file pointer position and set to beginning
    unsigned long current_position = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Get the first line of the file
    fgets(file_line, sizeof(file_line), fp);

    // Return the pointer to the original position
    fseek(fp, current_position, SEEK_SET);

    // Return the number of stacks (4 characters per stack)
    return strlen(file_line) - 1;

}

/*
void mark_from_left(char **tree_map, bool **visible_map, unsigned int num_rows, unsigned int num_cols) {
    for (int row_index = 1; row_index < num_rows - 1; row_index++) {
        // Get the tree line
        char tree_line[MAX_LINE_LEN];
        fgets(tree_line, sizeof(tree_line), tree_map);
        if (feof(tree_map)) { fseek(tree_map, 0, SEEK_SET); return; }


        tree_t leftmost_tree = { 0, tree_line[0] };
        for (int col_index = 1; col_index < num_cols - 2; col_index++) {
            // Parsing as string is fine
            if (tree_line[col_index] > leftmost_tree.height) {
                leftmost_tree.grid_value = col_index;
                leftmost_tree.height = tree_line[col_index];
                visible_map[row_index][leftmost_tree.grid_value] = true;
                if (tree_line[col_index] == '9') { break; }
            }
        }

    }
}*/


void init_visible_map(bool **visible_map, unsigned int num_rows, unsigned int num_cols) {
    for (int row_index = 0; row_index < num_rows; row_index++) {
        printf("a\n");
        for (int col_index = 0; col_index < num_cols; col_index++) {
            printf("%i, %i\n", row_index, col_index);
            if (row_index == 0 || row_index == num_rows - 1|| col_index == 0 || col_index == num_cols - 1) {
                printf("T\n");
                visible_map[row_index][col_index] = true;
            }
            else {
                printf("F\n");
                visible_map[row_index][col_index] = false;
            }
        }
    }
}


void init_tree_map(FILE *fp, char **tree_map, unsigned int num_rows, unsigned int num_cols) {
    for (int row_index = 0; row_index < num_rows; row_index++) {
        char *row_buffer = malloc(num_cols+2);
        fgets(row_buffer, num_cols + 2, fp);
        for (int col_index = 0; col_index < num_cols; col_index++) {
            tree_map[row_index][col_index] = row_buffer[col_index];
            if (row_buffer[col_index] == '\n') { printf("Uh oh @ %i, %i\n", row_index, col_index); exit(1); } 
            printf("%c", row_buffer[col_index]);
        }
        printf("\n");
        free(row_buffer);
    }
}


void display_tree_map(bool **visible_map, unsigned int num_rows, unsigned int num_cols) {
    for (int row_index = 0; row_index < num_rows; row_index++) {
        for (int col_index = 0; col_index < num_cols; col_index++) {
            printf("%d", visible_map[row_index][col_index]);
        }
        printf("\n");
    }
}


unsigned int count_visible_trees(bool **treemap, unsigned int num_rows, unsigned int num_cols) {
    unsigned int total = 0;
    for (int row_index = 0; row_index < num_rows; row_index++) {
        for (int col_index = 0; col_index < num_cols; col_index++) {
            if (treemap[row_index][col_index] == true) { total++; }
        }
    }
    return total;
}


int main(int argc, char **argv) {

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    unsigned int num_rows = get_num_rows(fp);
    unsigned int num_cols = get_num_cols(fp);

    printf("# rows: %u\n", num_rows);
    printf("# cols: %u\n", num_cols);

    char **tree_map = malloc(num_rows * num_cols * sizeof(char));
    for (int index = 0; index < num_rows; index++) {
        tree_map[index] = malloc(num_cols * sizeof(int));
    }
    init_tree_map(fp, tree_map, num_rows, num_cols);


    bool **visible_map = malloc(num_rows * num_cols * sizeof(int));  // TODO: FIx later?
    for (int index = 0; index < num_rows; index++) {
        visible_map[index] = malloc(num_cols * sizeof(int));
    }

    init_visible_map(visible_map, num_rows, num_cols);

    //mark_from_left(fp, visible_map, num_rows, num_cols);

    display_tree_map(visible_map, num_rows, num_cols);

    unsigned int total = count_visible_trees(visible_map, num_rows, num_cols);
    printf("Total trees visible: %u\n", total);
    
}
