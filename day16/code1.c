#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../shared/custom_limits.h"
#include "../shared/file_manip.h"
#include "../shared/str_manip.h"

#include "hash_table.h"


// TODO: Add documentation
void print_valve_info(hash_info_item_t valve) {
    printf("Valve %s w/ p=%i, connected to ", valve.key, valve.pressure);
    for (unsigned int conn_index = 0; conn_index < valve.num_connections; conn_index++) {
        if (conn_index != 0) { printf(", "); }
        printf("%s", valve.connections[conn_index]);
    }
    printf("\n");
}


// TODO: Add documemntation
void print_valve_table(hash_table_t table) {
    for (unsigned long index = 0; index < table.count; index++) {
        printf("%lu: ", index + 1);
        hash_info_item_t *current_item = (hash_info_item_t *)retrieve_by_hash(table, table.hashes[index]);
        print_valve_info(*current_item);
    }
}


// TODO: Add documemntation
void print_route_table(hash_table_t table) {
    for (unsigned long index = 0; index < table.count; index++) {
        printf("%lu: ", index + 1);
        hash_route_item_t *route = (hash_route_item_t *)retrieve_by_hash(table, table.hashes[index]);
        printf("%s <--> %s : %u\n", route->key_a, route->key_b, route->duration);
    }
}


// TODO: Add documentation
hash_table_t * parse_file(FILE *fp, unsigned long num_valves) {

    // Get the table
    hash_table_t *table = create_table(num_valves);

    // Iterate through the file
    while (true) {

        // Read the next line in the file
        char file_line[MAX_LINE_LEN];
        fgets(file_line, sizeof(file_line), fp);

        // Break if reached end of fie
        if (feof(fp)) { break; }

        // Get the first part of the line and parse for data
        char *flow_rate_str = strtok(file_line, ";");
        char key[3];
        int pressure;
        sscanf(flow_rate_str, "Valve %s has flow rate=%d", key, &pressure);

        // Get the second part of the line and parse for data
        char *connections_str = strtok(NULL, "\n");
        ltrim_chars(connections_str, 23);
        unsigned int num_connections = (strlen(connections_str) + 2) / 4;
        
        // Allocate memory for all the connections
        char **connections = malloc(num_connections * sizeof(char *));
        for (unsigned int index = 0; index < num_connections; index++) {
            connections[index] = malloc(3 * sizeof(char));
        }

        // Iterate through connection string names
        unsigned int index_connections = 0;

        while (true) {
            
            // Get the next connection
            char *connection;
            if (index_connections == 0) { connection = strtok(connections_str, ", "); }
            else { connection = strtok(NULL, ", "); }

            // Check if end of connections
            if (connection == NULL) { break; }

            // Add connection to stored array
            strncpy(connections[index_connections], connection, 3);
            index_connections++;

        }

        // Insert new connection into the table and then free the memory
        insert_info_item(table, key, pressure, connections, num_connections);
        for (unsigned int index = 0; index < num_connections; index++) {
            free(connections[index]);
        }
        free(connections);
    }

    // Return the populated table
    return table;

}


// TODO: Add documentation
/*
unsigned long find_max_pressure(hash_table_t table, hash_info_item_t *current, unsigned int time_remaining, unsigned long current_pressure, unsigned long pressure_per_min, hash_info_item_t *prev_room) {
    
    // Keep track of the maximum pressure so far
    unsigned long max_pressure = current_pressure;

    // Subtract time for movement/opening valve
    time_remaining--;
    max_pressure += pressure_per_min;

    if (time_remaining == 0) { return max_pressure; }


    // Simulate skipping and going to the next room (ignore previous)
    for (unsigned int index = 0; index < current->num_connections; index++) {
        char *next_room_key = current->connections[index];
        hash_info_item_t *next_room = retrieve_info_by_key(table, next_room_key);
        unsigned long path_pressure = find_max_pressure(table, next_room, time_remaining, max_pressure, pressure_per_min, current);
        max_pressure = path_pressure > max_pressure ? path_pressure : max_pressure;
    }

    // Return the current answer if not worth opening current valve or it already is open
    if (current->pressure == 0 || current->opened == true) { return max_pressure; }

    // Simulate opening the valve and moving on
    current->opened = true;
    time_remaining--;
    max_pressure += pressure_per_min;

    if (time_remaining == 0) { return max_pressure; }

    for (unsigned int index = 0; index < current->num_connections; index++) {
        char *next_room_key = current->connections[index];
        hash_info_item_t *next_room = retrieve_info_by_key(table, next_room_key);
        unsigned long path_pressure = find_max_pressure(table, next_room, time_remaining, max_pressure, pressure_per_min + current->pressure, current);
        max_pressure = path_pressure > max_pressure ? path_pressure : max_pressure;
    }

    return max_pressure;

}
*/


// TODO: Add documentation
unsigned int determine_shortest_route(hash_table_t info_table, hash_info_item_t *room, char* target_room, char **rooms_checked, unsigned int num_rooms_checked, unsigned int current_shortest) {

    unsigned int current_best = current_shortest;

    if (num_rooms_checked >= current_best) { return current_best; }
    if (!strcmp(room->key, target_room)) { return num_rooms_checked; }

    // Check if room checked yet
    for (unsigned int index = 0; index < num_rooms_checked; index++) {
        if (!strcmp(rooms_checked[index], target_room)) { return current_best; }
    }
    
    // Add to list of rooms checked
    num_rooms_checked++;
    rooms_checked = realloc(rooms_checked, num_rooms_checked * sizeof(char *));
    rooms_checked[num_rooms_checked - 1] = room->key;

    // Check other rooms
    for (unsigned int index = 0; index < room->num_connections; index++) {
        char *next_room_key = room->connections[index];
        hash_info_item_t *next_room = retrieve_info_by_key(info_table, next_room_key);
        unsigned int recent_duration = determine_shortest_route(info_table, next_room, target_room, rooms_checked, num_rooms_checked, current_best);
        current_best = recent_duration < current_best ? recent_duration : current_best;
    }

    return current_best;

}


// TODO: Add documentation
hash_table_t * calculate_routes(hash_table_t info_table, unsigned int num_valves) {

    // Create the table for storing route info
    hash_table_t *route_table = create_table(num_valves * num_valves);

    for (unsigned long index = 0; index < info_table.count - 1; index++) {
        hash_info_item_t *room = (hash_info_item_t *)retrieve_by_hash(info_table, info_table.hashes[index]);
        for (int subindex = 1; subindex < info_table.count; subindex++) {

            // Get the info of the connected room, skip if already populated
            hash_info_item_t *next_room = (hash_info_item_t *)retrieve_by_hash(info_table, info_table.hashes[subindex]);
            char *next_room_key = next_room->key;
            hash_route_item_t *target_room = retrieve_route_by_keys(*route_table, room->key, next_room_key);
            if (target_room != NULL) { continue; }

            // Allocate memory for the rooms checked, get shortest route
            char **rooms_checked = malloc(sizeof(char *));
            unsigned int route_duration = determine_shortest_route(info_table, room, next_room_key, rooms_checked, 0, UINT_MAX);

            // Insert the item, free memory
            insert_route_item(route_table, room->key, next_room_key, route_duration);
            free(rooms_checked);

        }
    }
    
    return route_table;
}


// TODO: Add documentation
int main(int argc, char **argv) {

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Get the number of valves
    unsigned int num_valves = get_num_rows(fp);

    // Create the hash table of valves
    hash_table_t *valve_table = parse_file(fp, num_valves);

    // Retrieve the starting location
    hash_info_item_t *start = retrieve_info_by_key(*valve_table, "AA");
    printf("Start: ");
    print_valve_info(*start);

    // Determine the shortest routes between rooms
    hash_table_t *route_table = calculate_routes(*valve_table, num_valves);
    print_route_table(*route_table);

}
