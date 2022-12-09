#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


// Definitions
#define MAX_LINE_LEN UCHAR_MAX


// Type definition of a tree
typedef struct GridTree {
    int grid_value;
    char height;
} tree_t;


/// @brief Get the number of rows in the forest
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

        num_rows++; }

    // Reset the file pointer position
    fseek(fp, current_position, SEEK_SET);

    // Return the number of rows
    return num_rows;

}


/// @brief Get the number of columns in the forest
/// @param fp A pointer to the open file
/// @return The number of columns
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


/// @brief Allocate memory for a char 2D matrix
/// @param input_matrix A pointer to a char** buffer
/// @param num_rows The number of rows in the matrix
/// @param num_cols The number of columns in the matrix
void allocate_char_matrix(char ***input_matrix, unsigned int num_rows, unsigned int num_cols) {
    (*input_matrix) = malloc(num_rows * sizeof(char *));
    for (int index = 0; index < num_rows; index++) {
        (*input_matrix)[index] = malloc(num_cols * sizeof(char));
    }
}


/// @brief Allocate memory for a bool 2D matrix
/// @param input_matrix A pointer to a bool** buffer
/// @param num_rows The number of rows in the matrix
/// @param num_cols The number of columns in the matrix
void allocate_bool_matrix(bool ***input_matrix, unsigned int num_rows, unsigned int num_cols) {
    (*input_matrix) = malloc(num_rows * sizeof(int *));
    for (int index = 0; index < num_rows; index++) {
        (*input_matrix)[index] = malloc(num_cols * sizeof(int));
    }
}


/// @brief Mark trees in the given buffer from the left side
/// @param tree_map A pointer to a char** 2D matrix
/// @param visible_map A pointer to a bool** 2D matrix to store the result
/// @param num_rows The number of rows in the matrix
/// @param num_cols The number of columns in the matrix
void calculate_score_from_left(char ***tree_map, bool ***visible_map, unsigned int num_rows, unsigned int num_cols) {
    tree_t leftmost_tree;
    for (int row_index = 0; row_index < num_rows; row_index++) {
        leftmost_tree.grid_value = 0;
        leftmost_tree.height = '0';
        for (int col_index = 0; col_index < num_cols; col_index++) {

            // Parsing as string is fine
            if (col_index == 0 || (*tree_map)[row_index][col_index] > leftmost_tree.height) {
                leftmost_tree.grid_value = col_index;
                leftmost_tree.height = (*tree_map)[row_index][col_index];
                (*visible_map)[row_index][leftmost_tree.grid_value] = true;
                if ((*tree_map)[row_index][col_index] == '9') { break; }
            }

        }

    }
}


/// @brief Rotate the given char** 2D matrix clockwise
/// @param array_map A pointer to a char** 2D matrix
/// @param num_rows The number of rows in the matrix
/// @param num_cols The number of columns in the matrix
void rotate_char_clockwise(char ***array_map, unsigned int num_rows, unsigned int num_cols) {
    char **array_copy;
    allocate_char_matrix(&array_copy, num_rows, num_cols);

    for (int row_index = 0; row_index < num_rows; row_index++) {
        for (int col_index = 0; col_index < num_cols; col_index++) {
            array_copy[row_index][col_index] = (*array_map)[(num_cols-1) - col_index][row_index];
        }
    }
    free(*array_map);
    *array_map = array_copy;
}


/// @brief Rotate the given bool** 2D matrix clockwise
/// @param array_map A pointer to a bool** 2D matrix
/// @param num_rows The number of rows in the matrix
/// @param num_cols The number of columns in the matrix
void rotate_bool_clockwise(bool ***array_map, unsigned int num_rows, unsigned int num_cols) {
    bool **array_copy;
    allocate_bool_matrix((bool ***)&array_copy, num_rows, num_cols);

    for (int row_index = 0; row_index < num_rows; row_index++) {
        for (int col_index = 0; col_index < num_cols; col_index++) {
            array_copy[row_index][col_index] = (*array_map)[(num_cols-1) - col_index][row_index];
        }
    }
    free(*array_map);
    *array_map = array_copy;

}


/// @brief Initialize the tree visibility matrix
/// @param visible_map A pointer to the bool** 2D matrix
/// @param num_rows The number of rows in matrix
/// @param num_cols The number of columns in the matrix
void init_visible_map(bool ***visible_map, unsigned int num_rows, unsigned int num_cols) {
    for (int row_index = 0; row_index < num_rows; row_index++) {
        for (int col_index = 0; col_index < num_cols; col_index++) {
            (*visible_map)[row_index][col_index] = false;
        }
    }
}


/// @brief Initialize the tree height matrix
/// @param fp A pointer to the open input file
/// @param tree_map A pointer to the char** 2D matrix
/// @param num_rows The number of rows in matrix
/// @param num_cols The number of columns in the matrix
void init_tree_map(FILE *fp, char ***tree_map, unsigned int num_rows, unsigned int num_cols) {
    for (int row_index = 0; row_index < num_rows; row_index++) {
        char *row_buffer = malloc(num_cols+2);
        fgets(row_buffer, num_cols + 2, fp);
        for (int col_index = 0; col_index < num_cols; col_index++) {
            (*tree_map)[row_index][col_index] = row_buffer[col_index];
            if (row_buffer[col_index] == '\n') {
                printf("Error!");
                exit(1);
            } 
        }
        free(row_buffer);
    }
}


/// @brief Display a tree map
/// @param visible_map The tree map to display
/// @param num_rows The number of rows in the matrix
/// @param num_cols The number of columns in the matrix
void display_tree_map(bool **visible_map, unsigned int num_rows, unsigned int num_cols) {
    for (int row_index = 0; row_index < num_rows; row_index++) {
        for (int col_index = 0; col_index < num_cols; col_index++) {
            printf("%d", visible_map[row_index][col_index]);
        }
        printf("\n");
    }
}


/// @brief Count the number of trees visible from the map
/// @param treemap The visibility map
/// @param num_rows The number of rows in the matrix
/// @param num_cols The number of columns in the matrix
/// @return The total number of trees visible
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

    char **tree_map;
    allocate_char_matrix(&tree_map, num_rows, num_cols);
    init_tree_map(fp, &tree_map, num_rows, num_cols);

    bool **visible_map;
    allocate_bool_matrix(&visible_map, num_rows, num_cols);
    init_visible_map(&visible_map, num_rows, num_cols);

    for (int index = 0; index < 4; index++) {
        calculate_score_from_left(&tree_map, &visible_map, num_rows, num_cols);
        rotate_char_clockwise(&tree_map, num_rows, num_cols);
        rotate_bool_clockwise(&visible_map, num_rows, num_cols);
    }

    unsigned int total = count_visible_trees(visible_map, num_rows, num_cols);
    printf("Total trees visible: %u\n", total);

    free(tree_map);
    free(visible_map);
    
}
