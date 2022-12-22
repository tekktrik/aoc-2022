#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "../shared/custom_limits.h"
#include "../shared/file_manip.h"


// Helpful definitions
#define MAP_INCR 50


// Type definition for different types of elements
typedef enum Element {
    SPACE = 0,
    ROCK = 'X',
    SAND = 'o',
    ENTRY = '+',
    GROUND = '='
} element_t;


// Type definition for the nodes of the rock paths
typedef struct ElementNode {
    int x;
    int y;
    struct ElementNode *next_node;
    element_t type;
} element_node_t;


// Type definition for a rock and sand map
typedef struct WallMap {
    unsigned int min_x;
    unsigned int min_y;
    unsigned int width;
    unsigned int height;
    char **map;
} wall_map_t;


// Type definition for direction to increase map dimensions
typedef enum Direction {LEFT, RIGHT} direction_t;


/// @brief Parse the rock segment from the file
/// @param fp Pointer to the open input file
/// @param num_segments Pointer to a variable keeping track of the number of segments
/// @return Pointer to an array of rock segments
element_node_t ** parse_rock_segments(FILE *fp, unsigned int num_segments) {

    // Allocate memory for the rock segments
    element_node_t **segments = malloc(num_segments * sizeof(element_node_t));
    unsigned int seg_index = 0;

    // Declare array for the read file line
    char file_line[MAX_LINE_LEN];

    // Iterate through file
    while (true) {

        // Read a line from the file
        fgets(file_line, sizeof(file_line), fp);

        // Break if reached end of file
        if (feof(fp)) { break; }

        // Allocate memory for the head rock node
        element_node_t *head = malloc(sizeof(element_node_t));
        element_node_t *current_node = head;
        current_node->type = ROCK;

        // Interate and create linked list of rock nodes
        unsigned int node_index = 0;
        bool end_flag = false;
        while (!end_flag) {

            // Get the next parseable string
            char *file_marker = node_index == 0 ? file_line : NULL;
            char *str_coords = strtok(file_marker, " -> ");

            // Allocate memory for next node in advance
            current_node->next_node = malloc(sizeof(element_node_t));

            if (str_coords[strlen(str_coords) - 1] == '\n') {
                end_flag = true;
                str_coords[strlen(str_coords) - 1] = '\0';
            }

            // Parse the string and fill in the node
            sscanf(str_coords, "%d,%d", &current_node->x, &current_node->y);

            // Move to next node and increase node index count
            if (end_flag) {
                free(current_node->next_node);
                current_node->next_node = NULL;
            }
            else {
                current_node = current_node->next_node;
                node_index++;
            }
        }

        segments[seg_index] = head;
        seg_index++;
    }

    // Return the rock segments
    return segments;
    
}


/// @brief Add a rock segment to the wall map
/// @param wall_map Pointer to the wall map
/// @param node_a The head node of the rock segment
/// @param node_b The tail node of the rock segment
void fill_rock_line(wall_map_t *wall_map, element_node_t *node_a, element_node_t *node_b) {
    for (int col_index = 0; col_index < wall_map->width; col_index++) {
        for (int row_index = 0; row_index < wall_map->height; row_index++) {
            int mod_x = wall_map->min_x + col_index;
            int mod_y = wall_map->min_y + row_index;
            if (mod_x == 500 && mod_y == 0) { wall_map->map[col_index][row_index] = ENTRY; }
            else if (
                    (
                        (mod_x >= node_a->x && mod_x <= node_b->x) ||
                        (mod_x <= node_a->x && mod_x >= node_b->x)
                    )
                    &&
                    (
                        (mod_y >= node_a->y && mod_y <= node_b->y) ||
                        (mod_y <= node_a->y && mod_y >= node_b->y)
                    )
            ) {
                wall_map->map[col_index][row_index] = ROCK;
            }
        }
    }
}


/// @brief Create a wall map
/// @param segments Pointer to an array of segments
/// @param num_segments The number of segments
/// @return Pointer to the wall map
wall_map_t * create_wall_map(element_node_t **segments, unsigned int num_segments) {

    // Determine dimensions of map
    unsigned int min_x = UINT_MAX;
    unsigned int max_x = 500;
    unsigned int min_y = 0;
    unsigned int max_y = 0;
    for (unsigned int segment_index = 0; segment_index < num_segments; segment_index++) {
        element_node_t *segment = segments[segment_index];
        while (true) {
            min_x = segment->x < min_x ? segment->x : min_x;
            max_x = segment->x > max_x ? segment->x : max_x;
            max_y = segment->y > max_y ? segment->y : max_y;
            if (segment->next_node == NULL) { break; }
            else { segment = segment->next_node; }
        }
    }
    unsigned int width = (max_x - min_x) + 3;
    unsigned int height = (max_y - min_y) + 3;

    // Allocate memory for the map structure and initialize
    wall_map_t *wall_map = malloc(sizeof(wall_map_t));
    wall_map->min_x = min_x - 1;
    wall_map->min_y = min_y;
    wall_map->width = width;
    wall_map->height = height;
    wall_map->map = malloc(width * sizeof(char *));
    for (int col_index = 0; col_index < width; col_index++) {
        wall_map->map[col_index] = calloc(height, sizeof(char));
    }

    // Add the rocks to the map
    for (unsigned int seg_index = 0; seg_index < num_segments; seg_index++) {
        element_node_t *current_segment = segments[seg_index];
        while (current_segment->next_node != NULL) {
            fill_rock_line(wall_map, current_segment, current_segment->next_node);
            current_segment = current_segment->next_node;
        }
    }

    // Add ground
    for (unsigned int gnd_index = 0; gnd_index < width; gnd_index++) {
        wall_map->map[gnd_index][height - 1] = GROUND;
    }

    // Return the map
    return wall_map;

}


/// @brief Print the wall map
/// @param wall_map The wall map
void print_map(wall_map_t wall_map) {
    for (int row_index = 0; row_index < wall_map.height; row_index++) {
        for (int col_index = 0; col_index < wall_map.width; col_index++) {
            switch (wall_map.map[col_index][row_index])
            {
            case SPACE:
                printf(".");
                break;
            case ROCK:
            case SAND:
            case ENTRY:
            case GROUND:
                printf("%c", wall_map.map[col_index][row_index]);
                break;
            default:
                printf("Error occurred!  Found: %c\n", wall_map.map[col_index][row_index]);
                printf("Exiting...\n");
                exit(1);
            }
        }
        printf("\n");
    }
}


/// @brief Increase the character map dimensions
/// @param wall_map Pointer to the wall map
/// @param direction The direction to increase
/// @param num_cols THe number of columns to add
void increase_map_dimensions(wall_map_t *wall_map, direction_t direction, unsigned int num_cols) {

    // Allocate memory for the increase 2D array and initialize
    char **new_map = malloc((wall_map->width + num_cols) * sizeof(char *));
    for (int col_index = 0; col_index < wall_map->width + num_cols; col_index++) {

        if ((col_index < num_cols && direction == LEFT) || (col_index >= wall_map->width && direction == RIGHT)) {
            new_map[col_index] = calloc(wall_map->height, sizeof(char));
        }
        else if (direction == LEFT) {
            new_map[col_index] = wall_map->map[col_index - num_cols];
        }
        else {
            new_map[col_index] = wall_map->map[col_index];
        }

    }

    // Free existig memory and use new 2D matrix
    free(wall_map->map);
    wall_map->map = new_map;
    wall_map->width += num_cols;
    if (direction == LEFT) { wall_map->min_x -= num_cols; }

    // Add ground to new map
    for (int gnd_index = 0; gnd_index < wall_map->width; gnd_index++) {
        wall_map->map[gnd_index][wall_map->height - 1] = GROUND;
    }

}


/// @brief Simulate the sand falling
/// @param wall_map Pointer to the wall map
/// @param start_node The start node (sand entry point)
/// @return Whether another round should be simulated
bool simulate_sand(wall_map_t *wall_map, element_node_t start_node) {

    int x_index = start_node.x - wall_map->min_x;
    int y_index = start_node.y - wall_map->min_y;
    int next_x_index = x_index;
    int next_y_index = y_index;

    while (true) {

        // Check if map needs to increase
        if (x_index - 1 == 0) { // Increase left side
            increase_map_dimensions(wall_map, LEFT, MAP_INCR);
            x_index += MAP_INCR;
        }
        else if (x_index + 1 == wall_map->width - 1) { // Increase right side
            increase_map_dimensions(wall_map, RIGHT, MAP_INCR);
        }
    
        // Check next movement of sand
        if (wall_map->map[x_index][y_index + 1] == SPACE) {
            next_y_index++;
        }
        else if (wall_map->map[x_index - 1][y_index + 1] == SPACE) {
            next_y_index++;
            next_x_index--;
        }
        else if (wall_map->map[x_index + 1][y_index + 1] == SPACE) {
            next_y_index++;
            next_x_index++;
        }
        else {  // No movement possible, return ready for more sand
            return wall_map->map[x_index][y_index] != ENTRY;
        }

        // Erase sand in its current location and move to the next
        wall_map->map[x_index][y_index] = (
            (x_index + wall_map->min_x == start_node.x) && (y_index + wall_map->min_y == start_node.y) ? 
            ENTRY : 
            SPACE
        );
        wall_map->map[next_x_index][next_y_index] = SAND;

        // Update current sand location
        x_index = next_x_index;
        y_index = next_y_index;

    }
}


/// @brief Main program
/// @param argc The number of arguments
/// @param argv [Name of program, Input file path]
/// @return Resulting exit code
int main(int argc, char **argv) {

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Get the number of rock segments
    unsigned int num_segments = get_num_rows(fp);

    // Get the rock segments
    element_node_t **segments = parse_rock_segments(fp, num_segments);

    // Create a 2D map with the segments
    wall_map_t *wall_map = create_wall_map(segments, num_segments);

    // Simulate the sand falling process
    element_node_t sand_node = {500, 0, NULL, SAND};
    unsigned long units_sand = 1;
    while (simulate_sand(wall_map, sand_node)) { units_sand++; } 

    // Print amount of sand dropped
    printf("Amount of sand dropped: %lu\n", units_sand);


    // Return exit code 0 for success
    return 0;
}
