#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


// Define max line length
#ifndef MAX_LINE_LEN
    #include <limits.h>
    #define MAX_LINE_LEN UCHAR_MAX
#endif


typedef struct Node {
    unsigned int x;
    unsigned int y;
} node_t;


typedef struct NodePool {
    unsigned int num_nodes;
    node_t *pool;
} nodepool_t;


typedef struct Map {
    char **map;
    unsigned int height;
    unsigned int width;
} map_t;


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


/// @brief Get the number of columns on the elevation map
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

    // Return the number of columns
    return strlen(file_line) - 1;

}


/// @brief Load the map from the input file
/// @param fp A pointer to the file to read
/// @param map A pointer to the map to load into
void init_map(FILE *fp, map_t *map) {
    for (unsigned int row_index = 0; row_index < map->height; row_index++) {
        for (unsigned int col_index = 0; col_index < map->width; col_index++) {
            map->map[row_index][col_index] = (char)fgetc(fp);
        }
        fgetc(fp);
    }
}


/// @brief Print the map
/// @param map The map
void print_map(map_t map) {
    for (unsigned int row_index = 0; row_index < map.height; row_index++) {
        for (unsigned int col_index = 0; col_index < map.width; col_index++) {
            printf("%c", map.map[row_index][col_index]);
        }
        printf("\n");
    }
}


/// @brief Get a specific node based on the symbol
/// @param map The map
/// @param symbol The symbol (character) to search for
/// @return The corresponding node
node_t get_node(map_t map, char symbol) {
    for (unsigned int row_index = 0; row_index < map.height; row_index++) {
        for (unsigned int col_index = 0; col_index < map.width; col_index++) {
            if (map.map[row_index][col_index] == symbol) {
                node_t start_node = {row_index, col_index};
                return start_node;
            }
        }
    }
}


/// @brief Get the start node
/// @param map The map
/// @return The start node
node_t get_start_node(map_t map) {
    return get_node(map, 'S');
}


/// @brief Get the end node
/// @param map The map
/// @return The end node
node_t get_end_node(map_t map) {
    return get_node(map, 'E');
}

/// @brief Print a list of nodes in a nodepoool
/// @param nodepool A nodepool whose nodes to print
void print_nodepool(nodepool_t nodepool) {
    for (unsigned int index = 0; index < nodepool.num_nodes; index++) {
        printf("Node %u @ (%u, %u)\n", index + 1, nodepool.pool[index].x, nodepool.pool[index].y);
    }
}


/// @brief Check a node to see if it should be added to the next list of starts
/// @param map The map structure
/// @param node The node to check
/// @param x_delta The movement change in the x (across rows) direction
/// @param y_delta The movement change in the y (across columns) direction
/// @param nodepool A pointer to the current nodepool of nodes checked
/// @param new_nodepool A pointer to the nodepool of new eligible nodes discovered
void check_node(map_t map, node_t node, int x_delta, int y_delta, nodepool_t *nodepool, nodepool_t *new_nodepool) {

    // Check x bounds
    if (node.x + x_delta < 0 || node.x + x_delta >= map.height) { return; }
    if (node.y + y_delta < 0 || node.y + y_delta >= map.width) { return; }

    // Get the potnetial node to check
    node_t potential_node = {node.x + x_delta, node.y + y_delta};

    // Get character values
    char newchar = map.map[potential_node.x][potential_node.y];
    char oldchar = map.map[node.x][node.y];

    // Check character value difference is unacceptable
    if (newchar - oldchar > 1) { return; }

    // Check if node already in new nodepool
    for (unsigned int node_index = 0; node_index < new_nodepool->num_nodes; node_index++) {
        node_t new_node = new_nodepool->pool[node_index];
        if (potential_node.x == new_node.x && potential_node.y == new_node.y) { return; }
    }

    // Check if node already in past nodepool
    for (unsigned int node_index = 0; node_index < nodepool->num_nodes; node_index++) {
        node_t past_node = nodepool->pool[node_index];
        if (potential_node.x == past_node.x && potential_node.y == past_node.y) { return; }
    }

    new_nodepool->pool[new_nodepool->num_nodes] = potential_node;
    new_nodepool->num_nodes += 1;
}


/// @brief Find the shortest path for a generation of starting points
/// @param map The map structure
/// @param starts A nodepool of starts to check
/// @param end The end node to look for
/// @param nodepool A pointer to the current nodepool of points checked
/// @param generation The number representing the current generation of nodes
/// @return The length of the shortest path
unsigned int find_shortest_path_length(map_t map, nodepool_t starts, node_t end, nodepool_t *nodepool, unsigned int generation) {

    // Create nodepool for next generation
    node_t *new_starts = malloc(4 * starts.num_nodes * sizeof(node_t));
    nodepool_t new_nodepool = {0, new_starts};
    for (unsigned int node_index = 0; node_index < starts.num_nodes; node_index++) {

        // Get the current node
        node_t current_node = starts.pool[node_index];

        // Check all four directions
        check_node(map, current_node, 0, -1, nodepool, &new_nodepool);
        check_node(map, current_node, -1, 0, nodepool, &new_nodepool);
        check_node(map, current_node, 0, 1, nodepool, &new_nodepool);
        check_node(map, current_node, 1, 0, nodepool, &new_nodepool);

    }

    // Check new nodepool for end
    for (unsigned int node_index = 0; node_index < new_nodepool.num_nodes; node_index++) {
        node_t past_node = new_nodepool.pool[node_index];
        if (past_node.x == end.x && past_node.y == end.y) {
            return generation;
        }
    }

    // Combine nodepools
    for (unsigned int add_index = 0; add_index < new_nodepool.num_nodes; add_index++) {
        nodepool->pool[nodepool->num_nodes + add_index] = new_nodepool.pool[add_index]; 
    }
    nodepool->num_nodes += new_nodepool.num_nodes;

    // Increase the generation and check the new nodes
    generation++;
    find_shortest_path_length(map, new_nodepool, end, nodepool, generation);
}


/// @brief Fix the map endpoints 
/// @param map A pointer to the map object
/// @param start The start node
/// @param end The end node
void fix_map_endpoints(map_t *map, node_t start, node_t end) {
    map->map[start.x][start.y] = 'a' - 1;
    map->map[end.x][end.y] = 'z' + 1;
}


int main(int argc, char **argv) {

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Get the dimensions of the input map
    unsigned int height = get_num_rows(fp);
    unsigned int width = get_num_cols(fp);

    // Allocate memory for the map
    char **charmap = malloc(height * sizeof(char *));
    for (unsigned int row_index = 0; row_index < height; row_index++) {
        char *charmap_row = malloc(width * sizeof(char));
        charmap[row_index] = charmap_row;
    }

    // Load the map
    map_t map = {charmap, height, width};
    init_map(fp, &map);

    // Fnd the start and end nodes
    node_t start = get_start_node(map);
    node_t end = get_end_node(map);
    fix_map_endpoints(&map, start, end);

    // Print the start and end node locations
    printf("Start node @(%u, %u)\n", start.x, start.y);
    printf("End node @(%u, %u)\n", end.x, end.y);

    // Start a node pool
    node_t *checked_nodes = malloc(height * width * sizeof(node_t));
    checked_nodes[0] = start;
    nodepool_t nodepool = {1, checked_nodes};

    // Find the shortest length of the path
    unsigned int length = find_shortest_path_length(map, nodepool, end, &nodepool, 1);

    // Print length of path
    printf("Shortest path is %u steps\n", length);
}
