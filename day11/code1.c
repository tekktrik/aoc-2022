#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

// Define max line length
#ifndef MAX_LINE_LEN
    #include <limits.h>
    #define MAX_LINE_LEN UCHAR_MAX
#endif


/// The maximum size of a monkey's inventory
#define MAX_MONKEY_INVENTORY 100


/// Type definition for enumerated values representing the
/// operation to perform when updating worry levels
typedef enum {MULT, ADD} operation_t;


/// @brief Type definition for monkeys
typedef struct Monkey {
    unsigned int num_items;
    unsigned long long *items;
    unsigned int test_divisor;
    operation_t op_type;
    long op_value;
    bool op_old_mult;
    unsigned int test_true_monkey;
    unsigned int test_false_monkey;
    unsigned long long num_inspected;
} monkey_t;


/// @brief Trim characters on the left side of a string
/// @param file_line The string
/// @param n The number of characters to trim
void ltrim_chars(char *file_line, unsigned int n) {
    char *new_line = file_line + n;
    strncpy(file_line, new_line, strlen(new_line));
    file_line[strlen(new_line)] = '\0';
}


/// @brief Parse the input file for initilizing the monkeys
/// @param fp A pointer to the open file
/// @param monkeys A pointer to an array of monkey pointers
void parse_monkeys(FILE *fp, monkey_t ***monkeys) {

    // Allocate memory for the line buffer
    char file_line[MAX_LINE_LEN];

    // Declare the monkey arrays
    unsigned int num_monkeys = 0;
    monkey_t *monkey;

    // Keep track of line number
    unsigned int line_num = 0;

    // iterate through the file
    while (true) {

        // Read a line
        fgets(file_line, sizeof(file_line), fp);

        // Remove the trailing newline
        file_line[strcspn(file_line, "\n")] = 0;

        // Initialize monkey if needed
        switch (line_num % 7) {
        case 0:  // Monkey start
            monkey = malloc(sizeof(monkey_t));
            monkey->num_inspected = 0;
            break;
        case 1:  // Starting items;
            unsigned long long *monkey_items = malloc(MAX_MONKEY_INVENTORY * sizeof(unsigned long long));
            monkey->num_items = 0;
            char *end_ptr2;
            ltrim_chars(file_line, 18);
            while (true) {
                char *str_num;
                if (monkey->num_items == 0) {
                    str_num = strtok(file_line, ", ");
                }
                else {
                    str_num = strtok(NULL, ", ");
                }
                if (str_num == NULL) { break; }
                if (str_num[0] == ' ') { ltrim_chars(str_num, 1); }
                monkey_items[monkey->num_items] = strtoull(str_num, &end_ptr2, 10);
                monkey->num_items++;
            }
            monkey->items = monkey_items;
            break;
        case 2:  // Operation
            char *end_ptr;
            ltrim_chars(file_line, 23);
            char *op_str = strtok(file_line, " ");
            char *op_str_value = strtok(NULL, " ");
            bool op_self_ref = !strcmp(op_str_value, "old");
            unsigned long long op_value = op_self_ref ? 1 : strtoull(op_str_value, &end_ptr, 10);
            operation_t op_type = strcmp(op_str, "*") ? ADD : MULT;
            monkey->op_old_mult = op_self_ref;
            monkey->op_value = op_value;
            monkey->op_type = op_type;
            break;
        case 3:  // Test
            ltrim_chars(file_line, 21);
            monkey->test_divisor = atoi(file_line);
            break;
        case 4:  // If true
            ltrim_chars(file_line, 29);
            monkey->test_true_monkey = atoi(file_line);
            break;
        case 5:  // If false
            ltrim_chars(file_line, 30);
            monkey->test_false_monkey = atoi(file_line);
            break;
        case 6:  // Newline (end of monkey)
            num_monkeys++;
            if (num_monkeys == 1) {
                (*monkeys) = malloc(sizeof(monkey_t *));
            }
            else {
                (*monkeys) = realloc(*monkeys, num_monkeys * sizeof(monkey_t *));
            }
            (*monkeys)[num_monkeys - 1] = monkey;
            break;
        default:
            printf("Unable to parse monkeys, exiting!\n");
            exit(1);
        }

        if (feof(fp)) { return; }

        line_num++;

    }
}


/// @brief Get the number of monkeyst
/// @param fp A pointer to the open file
/// @return The number of rows
unsigned int get_num_monkeys(FILE *fp) {

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
    return (num_rows + 1) / 7;

}


/// @brief Update the worry level of an item
/// @param monkey The monkey with the item
/// @param item_level The item in questions
/// @return The new item level
unsigned long update_worry_level(monkey_t monkey, unsigned long long item_level) {
    unsigned long long value = monkey.op_old_mult ? item_level : monkey.op_value;
    switch (monkey.op_type)
    {
    case MULT:
        return (item_level * value) / 3;
        break;
    case ADD:
        return (item_level + value) / 3;
    default:
        printf("Error!\n");
        exit(1);
    }
}


/// @brief Test the worry level and decide on a monkey to pass to
/// @param monkey The monkey with the item
/// @param item_level The item level in question
/// @return The index of the monkey the item should go to
unsigned long long test_worry_level(monkey_t monkey, unsigned long long item_level) {
    return item_level % monkey.test_divisor ? monkey.test_false_monkey : monkey.test_true_monkey;
}


/// @brief Simulate a round of the monkeys
/// @param monkeys A pointer to an array of monkey pointers
/// @param num_monkeys The number of monkeys
void simulate_round(monkey_t ***monkeys, unsigned int num_monkeys) {
    for (unsigned int monkey_index = 0; monkey_index < num_monkeys; monkey_index++) {
        monkey_t *current_monkey = (*monkeys)[monkey_index];
        current_monkey->num_inspected += current_monkey->num_items;
        for (unsigned int item_index = 0; item_index < current_monkey->num_items; item_index++) {
            unsigned long long new_item_level = update_worry_level(*current_monkey, current_monkey->items[item_index]);
            unsigned int thrown_monkey_index = test_worry_level(*current_monkey, new_item_level);
            monkey_t *thrown_monkey = (*monkeys)[thrown_monkey_index];
            thrown_monkey->items[thrown_monkey->num_items] = new_item_level;
            thrown_monkey->num_items++;
        }
        current_monkey->num_items = 0;
    }
}


int main(int argc, char **argv) {

    // Open the input file
    FILE *fp = fopen(argv[1], "r");

    // Get the number of monkeys
    unsigned int num_monkeys = get_num_monkeys(fp);

    // Parse the monkeys from the file
    monkey_t **monkeys;
    parse_monkeys(fp, &monkeys);

    // Simulate monkmonkeysey actions
    printf("Simulating...\n");
    unsigned int num_rounds = atoi(argv[2]);
    for (unsigned int round_index = 0; round_index < num_rounds; round_index++) {
        simulate_round(&monkeys, num_monkeys);
    }

    unsigned long long monkey_a_max = 0;
    unsigned long long  monkey_b_max = 0;
    for (int monkey_index = 0; monkey_index < num_monkeys; monkey_index++) {
        unsigned long num_inspected = monkeys[monkey_index]->num_inspected;
        if (num_inspected > monkey_a_max) {
            monkey_b_max = monkey_a_max;
            monkey_a_max = num_inspected;
            continue;
        }
        if (num_inspected > monkey_b_max) {
            monkey_b_max = num_inspected;
        }
    }

    printf("Monkey business: %llu\n", monkey_a_max * monkey_b_max);

}