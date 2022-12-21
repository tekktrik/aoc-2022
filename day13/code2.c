#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// Define max line length
#ifndef MAX_LINE_LEN
    #include <limits.h>
    #define MAX_LINE_LEN UCHAR_MAX
#endif


// Type definition for enumerated values for list structure type
typedef enum { NUMBER, LIST } item_t;


// Type definition for enumerated values for list order determinations
typedef enum {
    UNDETERMINED = -1,
    UNORDERED = 0,
    ORDERED = 1
} order_t;


// Typed definition for list structure elements
typedef struct ListStructure {
    unsigned int num_items;
    item_t item_type;
    struct ListStructure **elements;
    long value;
} list_struct_t;


/// @brief Trim characters on the left side of a string
/// @param file_line The string
/// @param n The number of characters to trim
void ltrim_chars(char *file_line, unsigned int n) {
    char *new_line = file_line + n;
    memcpy(file_line, new_line, strlen(new_line));
    file_line[strlen(new_line)] = '\0';
}


/// @brief Compare two lists
/// @param list_a Pointer to the first list
/// @param list_b Pointer to the second list
/// @return The result of the list comparison
order_t compare_lists(list_struct_t *list_a, list_struct_t *list_b) {
    
    // Iterate through elements
    int current_num_a = list_a->num_items;
    int current_num_b = list_b->num_items;
    int max_elements = current_num_a >= current_num_b ? current_num_a : current_num_b;
    for (int index = 0; index <= max_elements; index++) {

        // See if either list is out of elements
        if (index == current_num_a && index < current_num_b) { return ORDERED; }
        if (index == current_num_b && index < current_num_a) { return UNORDERED; }
        if (index == current_num_a && index == current_num_b) { return UNDETERMINED; }

        list_struct_t *a_item = list_a->elements[index];
        list_struct_t *b_item = list_b->elements[index];

        // Check if numbers and compare if so
        if (a_item->item_type == NUMBER && b_item->item_type == NUMBER) {
            if (a_item->value < b_item->value) { return ORDERED; }
            if (a_item->value > b_item->value) { return UNORDERED; }
            continue;
        }

        // Create a nested list for the number
        list_struct_t *sub_list = malloc(sizeof(list_struct_t));
        int mod_status = 0;
        if (a_item->item_type == NUMBER && b_item->item_type == LIST) {
            mod_status = 1;
            a_item->elements = malloc(sizeof(list_struct_t *));
            a_item->elements[0] = sub_list;
            a_item->elements[0]->item_type = NUMBER;
            a_item->elements[0]->num_items = 1;
            a_item->elements[0]->value = a_item->value;
            a_item->item_type = LIST;
        }
        else if (a_item->item_type == LIST && b_item->item_type == NUMBER) {
            mod_status = -1;
            b_item->elements = malloc(sizeof(list_struct_t *));
            b_item->elements[0] = sub_list;
            b_item->elements[0]->item_type = NUMBER;
            b_item->elements[0]->num_items = 1;
            b_item->elements[0]->value = b_item->value;
            b_item->item_type = LIST;
        }

        // Compare the two lists
        order_t sub_result = compare_lists(a_item, b_item);
        if (mod_status == 1) {
            a_item->item_type = NUMBER;
            a_item->value = a_item->elements[0]->value;
            free(a_item->elements);
        }
        else if (mod_status == -1) {
            b_item->item_type = NUMBER;
            b_item->value = b_item->elements[0]->value;
            free(b_item->elements);
        }
        free(sub_list);
        if (sub_result != UNDETERMINED) { return sub_result; }

    }
}


/// @brief Parse a character string into a list structure
/// @param file_line The string to parse
/// @return A pointer to the list structure
list_struct_t * parse_list_string(char *file_line) {
    
    // Allocate memories and prepare the new list
    list_struct_t *new_list = malloc(sizeof(list_struct_t));
    list_struct_t **new_elements = malloc(sizeof(list_struct_t *));
    new_list->elements = new_elements;
    new_list->item_type = LIST;
    new_list->num_items = 0;
    new_list->value = 0;

    // Iterate through this level of the is
    while (true) {

        switch (file_line[0])
        {

        case ']':  // End of list

            // Trim character and return the list
            ltrim_chars(file_line, 1);
            return new_list;

        case ',':  // Next element

            // Trim the character and continue parsing
            ltrim_chars(file_line, 1);
            break;

        case '[':  // Another sublist
            
            // Add to tally of number of elements and trim the character
            new_list->num_items++;
            ltrim_chars(file_line, 1);

            // Recursively parse list
            list_struct_t *sub_list = parse_list_string(file_line);
            sub_list->item_type = LIST;

            // Reallocate memory and add as new element
            if (new_list->num_items > 1) {
                new_list->elements = realloc(new_list->elements, new_list->num_items * sizeof(list_struct_t *));
            }
            new_list->elements[new_list->num_items - 1] = sub_list;
            break;

        case '\n':  // End of string (return the list)
            return new_list;

        default:  // Assume it's a number

            // Add to tally of number of elements
            new_list->num_items++;

            // Allocate memory for this value as a list structure type
            list_struct_t *sub_number = malloc(sizeof(list_struct_t));
            sub_number->num_items = 1;
            sub_number->item_type = NUMBER;
            sub_number->value = 0;

            // Parse the digits into a number
            while (file_line[0] >= '0' && file_line[0] <= '9') {
                sub_number->value *= 10;
                sub_number->value += (file_line[0] - '0');
                ltrim_chars(file_line, 1);
            }

            // Reallocate memory and add as new element
            if (new_list->num_items > 1) {
                new_list->elements = realloc(new_list->elements, new_list->num_items * sizeof(list_struct_t *));
            }
            new_list->elements[new_list->num_items - 1] = sub_number;
            break;
        }
    }
}

/// @brief Parse a line from the file into a list structure
/// @param fp A pointer to the file, at the line to parse
/// @return A pointer to the list structure
list_struct_t * parse_list_structure(FILE *fp) {

    // Get a buffer to store characters and read line
    char file_line[MAX_LINE_LEN];
    fgets(file_line, sizeof(file_line), fp);

    // Parse the string into the list structure
    ltrim_chars(file_line, 1);
    list_struct_t *new_list_struct = parse_list_string(file_line);

}


/// @brief Insert a list into the list array
/// @param new_list Pointer to the list to insert
/// @param ordered_lists Pointer to an array of ordered list pointers
/// @param insert_index The index at which to insert the new list
/// @param num_elements The number of elements in the new list
void insert_into_list(list_struct_t *new_list, list_struct_t ***ordered_lists, unsigned long insert_index, unsigned long num_elements) {
    for (unsigned long index = num_elements; index > insert_index; index--) {
        (*ordered_lists)[index] = (*ordered_lists)[index - 1];
    }
    (*ordered_lists)[insert_index] = new_list;
}


/// @brief Add a new list to the ordered list
/// @param new_list Pointer to the new list
/// @param ordered_lists Pointer to an array of ordered list pointers
/// @param num_elements The number of elements in the new list
/// @return The index (starting from 1) at which the new list was added
unsigned long add_to_ordered_list(list_struct_t *new_list, list_struct_t ***ordered_lists, unsigned long num_elements) {
    if (num_elements == 0) {
        (*ordered_lists)[0] = new_list;
        return 1;
    }
    else {
        (*ordered_lists) = realloc(*ordered_lists, (num_elements + 1) * sizeof(list_struct_t *));
        for (int list_index = 0; list_index < num_elements; list_index++) {

            // Compare the two lists and insert if applicable
            order_t result = compare_lists(new_list, (*ordered_lists)[list_index]);
            if (result == ORDERED) {
                insert_into_list(new_list, ordered_lists, list_index, num_elements);
                return list_index + 1;
            }

        }
        (*ordered_lists)[num_elements] = new_list;
        return num_elements + 1;
    }
}


/// @brief Main program
/// @param argc The number of arguments
/// @param argv [Name of program, Path to input file]
/// @return The exit code result
int main(int argc, char **argv) {

    // Declare variable for holder ordered lists
    list_struct_t **ordered_lists = malloc(sizeof(list_struct_t *));

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Assign variables to keep track of total
    unsigned int line_number = 0;
    int index = 1;
    unsigned long marker_product = 1;

    // Iterate through the file
    while (true) {

        // Break if at end of file
        if (feof(fp)) { break; }

        line_number++;
        if (line_number % 3 == 0) {
            char temp_buffer[2];
            fgets(temp_buffer, sizeof(temp_buffer), fp);
            continue;
        }

        // Parse the lists (and discard empty line)
        list_struct_t *new_list = parse_list_structure(fp);

        // Order the lists
        //printf("Returned %lu\n", add_to_ordered_list(new_list, &ordered_lists, index - 1));
        add_to_ordered_list(new_list, &ordered_lists, index - 1);

        // Increase the index number
        index++;

    }

    // Find the location of the markers [[2]] and [[6]]
    int markers[2] = {2, 6};

    for (int marker_index = 0; marker_index < 2; marker_index++) {

        list_struct_t **subsubmarker = malloc(sizeof(list_struct_t *));
        list_struct_t *inner_subsubmarker = malloc(sizeof(list_struct_t));
        subsubmarker[0] = inner_subsubmarker;
        subsubmarker[0]->item_type = NUMBER;
        subsubmarker[0]->num_items = 1;
        subsubmarker[0]->value = markers[marker_index];

        list_struct_t **submarker = malloc(sizeof(list_struct_t *));
        list_struct_t *inner_submarker = malloc(sizeof(list_struct_t));
        submarker[0] = inner_submarker;
        submarker[0]->item_type = LIST;
        submarker[0]->num_items = 1;
        submarker[0]->elements = subsubmarker;

        list_struct_t *marker = malloc(sizeof(list_struct_t));
        marker->item_type = LIST;
        marker->num_items = 1;
        marker->elements = submarker;

        unsigned long ordering_result = add_to_ordered_list(marker, &ordered_lists, index - 1);
        marker_product *= ordering_result;
        index++;
    }

    // Print the product of the marker indices
    printf("Product of marker indices: %lu\n", marker_product);

    // Return exit code 0 for success
    return 0;

}