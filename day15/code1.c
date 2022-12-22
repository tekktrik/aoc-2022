#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


// TODO: Change to include later
#include <limits.h>
#define MAX_LINE_LEN UCHAR_MAX


// TODO: Add documentation
typedef enum SignalType {EMPTY, SENSOR, BEACON, SIGNAL} sigtype_t;


// TODO: Add documentation
typedef struct Signal {
    long x;
    long y;
    sigtype_t signal_type;
} signal_t;


// TODO: Add documentation
typedef struct SignalPair {
    signal_t *sensor;
    signal_t *beacon;
} sigpair_t;


// TODO: Add documentation
typedef struct SignalMap {
    long min_x;
    long min_y;
    unsigned long width;
    unsigned long height;
    sigtype_t **map;
} sigmap_t;


// TODO: Add documentation
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

        // Allocate memory for the next signal pair
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


// TODO: Add documentation
void print_signal(signal_t signal) {
    printf("%i @ (%li, %li)\n", signal.signal_type, signal.x, signal.y);
}


// TODO: Add documentation
void print_signal_pairs(sigpair_t *signal_pairs, unsigned int num_pairs) {
    for (unsigned int index = 0; index < num_pairs; index++) {
        printf("Index %u:\n");
        print_signal(*signal_pairs[index].sensor);
        print_signal(*signal_pairs[index].beacon);
    }
}


// TODO: Add documentation
int main(int argc, char **argv) {

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Get the list of signals and beacon pairs
    sigpair_t *signal_pairs;
    unsigned int num_pairs;
    get_signal_pairs(fp, &signal_pairs, &num_pairs);

    // Print the signal pairs
    print_signal_pairs(signal_pairs, num_pairs);

    // Calculate the bounds of the map based on distances
    // TODO: Add calculation

}
