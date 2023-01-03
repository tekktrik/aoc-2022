#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


// Define max line length
#ifndef MAX_LINE_LEN
    #include <limits.h>
    #define MAX_LINE_LEN UCHAR_MAX
#endif


// Type definition for enumerated values representing signal types
typedef enum SignalType {EMPTY, SENSOR, BEACON, SIGNAL} sigtype_t;


// Type definition for structure representing singal information
typedef struct Signal {
    long x;
    long y;
    sigtype_t signal_type;
} signal_t;


// Type definition for struct for holding information about sensor/beacon pairs
typedef struct SignalPair {
    signal_t *sensor;
    signal_t *beacon;
} sigpair_t;


// Type definition for struct representing the signal map
typedef struct SignalMap {
    long min_x;
    long min_y;
    unsigned long width;
    unsigned long height;
    sigpair_t *signal_pairs;
    unsigned int num_pairs;
} sigmap_t;


/// @brief Get the signal pairs from the input file
/// @param fp Pointer to the open input file
/// @param signal_pairs Pointer to array for storing the signal pairs
/// @param num_pairs Pointer to the variable storing the number of signal pairs parsed
void get_signal_pairs(FILE *fp, sigpair_t **signal_pairs, unsigned int *num_pairs) {

    // Keep track of number of pairs
    *num_pairs = 0;

    // Declare variable for holding file line contents
    char file_line[MAX_LINE_LEN];
    
    // Iterate through the file
    while (true) {

        // Read a line from the file
        fgets(file_line, sizeof(file_line), fp);

        // Break if reached end of file
        if (feof(fp)) { break; }

        // Increment the number of signal pairs
        (*num_pairs)++;

        // Allocate memory for signal pairs as necessary
        if (*num_pairs == 1) {
            *signal_pairs = malloc(sizeof(sigpair_t));
        }
        else {
            *signal_pairs = realloc(*signal_pairs, *num_pairs * sizeof(sigpair_t));
        }

        // Allocate memory for the next
        signal_t *sensor = malloc(sizeof(signal_t));
        signal_t *beacon = malloc(sizeof(signal_t));
        sensor->signal_type = SENSOR;
        beacon->signal_type = BEACON;
        sigpair_t signal_pair = {sensor, beacon};

        // Parse the line for the needed information
        sscanf(
            file_line,
            "Sensor at x=%li, y=%li: closest beacon is at x=%li, y=%li\n",
            &signal_pair.sensor->x,
            &signal_pair.sensor->y,
            &signal_pair.beacon->x,
            &signal_pair.beacon->y
        );

        // Insert the new signal pair
        (*signal_pairs)[*num_pairs - 1] = signal_pair;

    }

}


/// @brief Print a signal in text form
/// @param signal Signal to print
void print_signal(signal_t signal) {
    printf("%i @ (%li, %li)\n", signal.signal_type, signal.x, signal.y);
}


/// @brief Print the pairs of signals
/// @param signal_pairs List of signal pairs
/// @param num_pairs The number of signal pairs
void print_signal_pairs(sigpair_t *signal_pairs, unsigned int num_pairs) {
    for (unsigned int index = 0; index < num_pairs; index++) {
        printf("Index %u:\n", index);
        print_signal(*signal_pairs[index].sensor);
        print_signal(*signal_pairs[index].beacon);
    }
}


/// @brief Get the Manhattan distance between a sensor and an (x, y) coordinate
/// @param sensor The sensor signal struct
/// @param x X coordinate of the point
/// @param y Y coordinate of the point
/// @return The Manhattan distance
unsigned long get_manhattan_distance_coord(signal_t sensor, long x, long y) {
    // Check if point is within range of sensor/beacon
    unsigned long point_x_diff = abs(sensor.x - x);
    unsigned long point_y_diff = abs(sensor.y - y);
    return point_x_diff + point_y_diff;
}


/// @brief Get the Manhattan distance between a sensor and an (x, y) coordinate
/// @param sensor The sensor signal struct
/// @param point The point signal struct
/// @return The Manhattan distance
unsigned long get_manhattan_distance_point(signal_t sensor, signal_t point) {
    return get_manhattan_distance_coord(sensor, point.x, point.y);
}


/// @brief Get the signal type of a point
/// @param signal_map The signal map
/// @param x_indexed The x index of the x coordiate in the map
/// @param y_indexed The y index of the y coordinate in the map
/// @return The singal type of the provided point
sigtype_t get_signal_type(sigmap_t signal_map, long x_indexed, long y_indexed) {

    // Convert to map coordinates
    long x = signal_map.min_x + x_indexed;
    long y = signal_map.min_y + y_indexed;

    // Iterate through signal pairs
    for (unsigned int index = 0; index < signal_map.num_pairs; index++) {

        // Get the sensor and beacon
        signal_t *sensor = signal_map.signal_pairs[index].sensor;
        signal_t *beacon = signal_map.signal_pairs[index].beacon;

        // Check if point is sensor or beacon
        if (x == sensor->x && y == sensor->y) { return SENSOR; }
        if (x == beacon->x && y == beacon->y) { return BEACON; }

        // Check if point is within range of sensor/beacon
        unsigned long point_distance = get_manhattan_distance_coord(*sensor, x, y);
        unsigned long sig_distance = get_manhattan_distance_point(*sensor, *beacon);
        if (point_distance <= sig_distance) { return SIGNAL; }

    }

    // Return that point is empty
    return EMPTY;
}


/// @brief Get the signal map
/// @param signal_pairs List of signal pairs
/// @param num_pairs The number of signal pairs
/// @return Pointer to the signal map
sigmap_t * get_map(sigpair_t *signal_pairs, unsigned int num_pairs) {

    // Keep track of maxes and mins
    sigmap_t *signal_map = malloc(sizeof(sigmap_t));
    signal_map->signal_pairs = signal_pairs;
    signal_map->num_pairs = num_pairs;
    long min_x = signal_pairs[0].beacon->x;
    long min_y = signal_pairs[0].beacon->y;
    long max_x = min_x;
    long max_y = min_y;
    
    // Iterate through pairs looking for maxes and mins
    for (unsigned int index = 0; index < num_pairs; index++) {

        signal_t *sensor = signal_pairs[index].sensor;
        signal_t *beacon = signal_pairs[index].beacon;

        unsigned long signal_distance = get_manhattan_distance_point(*sensor, *beacon);
        long sensed_min_x = sensor->x - signal_distance;
        long sensed_max_x = sensor->x + signal_distance;
        long sensed_min_y = sensor->y - signal_distance;
        long sensed_max_y = sensor->y + signal_distance;

        min_x = sensed_min_x < min_x ? sensed_min_x : min_x;
        max_x = sensed_max_x > max_x ? sensed_max_x : max_x;
        min_y = sensed_min_y < min_y ? sensed_min_y : min_y;
        max_y = sensed_max_y > max_y ? sensed_max_y : max_y;

    }
    signal_map->min_x = min_x;
    signal_map->min_y = min_y;
    signal_map->width = (max_x - min_x) + 1;
    signal_map->height = (max_y - min_y) + 1;

    // Return the signal map
    return signal_map;

}


/// @brief Print a row from the signal map
/// @param signal_map The signal map
/// @param y The y coordinate of the row
void print_signal_map_row(sigmap_t signal_map, unsigned long y) {
    for (unsigned long col_index = 0; col_index < signal_map.width; col_index++) {
        switch (get_signal_type(signal_map, col_index, y - signal_map.min_y))
        {
        case SENSOR:
            printf("S");
            break;
        case BEACON:
            printf("B");
            break;
        case SIGNAL:
            printf("#");
            break;
        case EMPTY:
            printf(".");
            break;
        }
    }
    printf("\n");
}


/// @brief Main function
/// @param argc The number of arguments
/// @param argv [Name of program, Filepath to input file, Row coordinate to check for signals]
/// @return The exit code
int main(int argc, char **argv) {

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Get the list of signals and beacon pairs
    sigpair_t *signal_pairs;
    unsigned int num_pairs;
    get_signal_pairs(fp, &signal_pairs, &num_pairs);

    // Get the signal map
    sigmap_t *signal_map = get_map(signal_pairs, num_pairs);

    // Get the number of places a signal can't be for a specified row
    int specified_row = atoi(argv[2]);
    unsigned long long signals = 0;
    unsigned long long empties = 0;
    unsigned long long beacons = 0;
    unsigned long long sensors = 0;
    for (unsigned long col_index = 0; col_index < signal_map->width; col_index++) {
        switch (get_signal_type(*signal_map, col_index, specified_row - signal_map->min_y))
        {
        case SIGNAL:
            signals++;
            break;
        case EMPTY:
            empties++;
            break;
        case BEACON:
            beacons++;
            break;
        case SENSOR:
            sensors++;
            break;
        }
    }
    printf("Signals: %llu, Sensors: %llu, Beacons: %llu, Empty: %llu\n", signals, sensors, beacons, empties);

    // Return exit code 0 for success
    return 0;

}
