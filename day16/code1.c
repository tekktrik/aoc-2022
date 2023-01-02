#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../shared/custom_limits.h"
#include "../shared/file_manip.h"
#include "../shared/str_manip.h"

#include "hash_table.h"


// TODO: Add documentation
void print_valve_info(hash_info_item_t valve) {
    printf("Valve %s w/ p=%i, connected to ", valve.key, valve.rate);
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
unsigned long calculate_max_release(hash_table_t *info_table, hash_table_t *route_table, char *current_valve, char **path_history, unsigned long history_length, unsigned int time_left, unsigned long current_released, unsigned int currente_rate, char *start_room) {

    // Keep track of the maximum pressure released and rate
    unsigned long max_released = current_released;

    // Get all the unopened connected room valves
    hash_info_item_t *current_room = retrieve_info_by_key(*info_table, current_valve);
    // TODO: Fix to use all connections, not just this room's immediately connected
    unsigned int num_connections = route_table->count;
    char **unopened_valves;
    unsigned int num_unopened_valves = 0;
    for (int index = 0; index < num_connections; index++) {
        unsigned long long check_valve_hash = route_table->hashes[index];
        hash_route_item_t *connected_room = retrieve_by_hash(*route_table, check_valve_hash);
        if (!strcmp(connected_room->key_a, current_valve) || !strcmp(connected_room->key_b, current_valve)) {
            char *search_room_str = !strcmp(connected_room->key_a, current_valve) ? connected_room->key_b : connected_room->key_a;
            hash_info_item_t *search_room = retrieve_info_by_key(*info_table, search_room_str);
            if (search_room->opened) { continue; }
            num_unopened_valves++;
            if (num_unopened_valves == 1) {
                unopened_valves = malloc(sizeof(char *));
            }
            else {
                unopened_valves = realloc(unopened_valves, num_unopened_valves * sizeof(char *));
            }
            unopened_valves[num_unopened_valves - 1] = !strcmp(connected_room->key_a, current_valve) ? connected_room->key_b : connected_room->key_a;
        }
    }

    // Return final pressure if no open valves
    if (num_unopened_valves == 0) { return current_released + (currente_rate * time_left); }

    // Get a copy of the path history to use, add current room
    history_length++;
    char **current_path_history = malloc(history_length * sizeof(char *));
    for (unsigned long index = 0; index < history_length - 1; index++) {
        current_path_history[index] = path_history[index];
    }
    current_path_history[history_length - 1] = current_valve;

    // Check the unopened valves
    for (int index = 0; index < num_unopened_valves; index++) {

        // Handle if time is less than the move for the valve
        hash_route_item_t *current_route = retrieve_route_by_keys(*route_table, current_valve, unopened_valves[index]);
        printf("%s --> %s \n", current_valve, unopened_valves[index]);
        unsigned int time_to_arrive = current_route->duration;
        if (time_left <= time_to_arrive + 2) {
            unsigned long new_released = current_released + (currente_rate * time_left);
            max_released = new_released > max_released ? new_released : max_released;
        }
        // Handle move if there is enought time
        else {
            unsigned long new_released = current_released + (currente_rate * (time_to_arrive + 1));
            unsigned int new_time_left = time_left - (time_to_arrive + 1);
            hash_info_item_t *next_room = retrieve_info_by_key(*info_table, unopened_valves[index]);
            unsigned int new_rate = currente_rate + next_room->rate;
            next_room->opened = true;
            unsigned long nested_new_released = calculate_max_release(info_table, route_table, unopened_valves[index], current_path_history, history_length, new_time_left, new_released, new_rate, start_room);
            max_released = nested_new_released > max_released ? nested_new_released : max_released;
            next_room->opened = false;
        }

    }

    // Return the maximum released pressure
    return max_released;
    
}


// TODO: Add documentation
unsigned int determine_shortest_route(hash_table_t info_table, hash_info_item_t *room, char* target_room, char **rooms_checked, unsigned int num_rooms_checked, unsigned int current_best) {

    unsigned int tracked_best = current_best;

    if (!strcmp(room->key, target_room)) { return num_rooms_checked; }
    if (num_rooms_checked > tracked_best) { return tracked_best; }

    // Check if room checked yet
    for (unsigned int index = 0; index < num_rooms_checked; index++) {
        if (!strcmp(rooms_checked[index], room->key)) { return tracked_best; }
    }
    
    // Add to list of rooms checked
    num_rooms_checked++;
    char **new_rooms_checked = malloc(num_rooms_checked * sizeof(char *));
    for (unsigned int index = 0; index < num_rooms_checked - 1; index++) {
        new_rooms_checked[index] = rooms_checked[index];
    }
    new_rooms_checked[num_rooms_checked - 1] = room->key;

    // Check other rooms
    for (unsigned int index = 0; index < room->num_connections; index++) {
        char *next_room_key = room->connections[index];
        hash_info_item_t *next_room = retrieve_info_by_key(info_table, next_room_key);
        unsigned int recent_duration = determine_shortest_route(info_table, next_room, target_room, new_rooms_checked, num_rooms_checked, tracked_best);
        tracked_best = recent_duration < tracked_best ? recent_duration : tracked_best;
    }

    // Free the memory and return the tracked best
    free(new_rooms_checked);
    return tracked_best;

}


// TODO: Add documentation
hash_table_t * calculate_routes(hash_table_t info_table, char *start_room, unsigned int num_valves) {

    // Calculate table size
    unsigned long table_size = 0;
    for (unsigned long index = 1; index < num_valves; index++) {
        table_size += index;
    }

    // Create the table for storing route info
    hash_table_t *route_table = create_table(table_size);

    for (unsigned long index = 0; index < info_table.size; index++) {
        hash_info_item_t *room = (hash_info_item_t *)retrieve_by_hash(info_table, info_table.hashes[index]);
        for (int subindex = index + 1; subindex < info_table.count; subindex++) {

            // Get the info of the connected room, skip if already populated or either has zero flowrate (except start)
            hash_info_item_t *next_room = (hash_info_item_t *)retrieve_by_hash(info_table, info_table.hashes[subindex]);
            if (
                (room->rate == 0 && strcmp(room->key, start_room)) ||
                (next_room->rate == 0 && strcmp(next_room->key, start_room))
            ) {
                continue;
            }
            char *next_room_key = next_room->key;
            hash_route_item_t *room_route = retrieve_route_by_keys(*route_table, room->key, next_room_key);
            if (room_route != NULL) { continue; }

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
    print_valve_table(*valve_table);

    // Retrieve the starting location
    hash_info_item_t *start = retrieve_info_by_key(*valve_table, "AA");
    printf("Start: ");
    print_valve_info(*start);

    // Determine the shortest routes between rooms
    hash_table_t *route_table = calculate_routes(*valve_table, "AA", num_valves);
    print_route_table(*route_table);

    // Calculate the maximum pressure released
    char **path_history;
    unsigned long max_release = calculate_max_release(valve_table, route_table, "AA", path_history, 0, 30, 0, 0, "AA");
    printf("Max released is %lu\n", max_release);

}
