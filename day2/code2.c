#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Declare variables
FILE *fp;
char readBuffer[10];

// Declare variables for win, lose, and draw
uint8_t winPoints = 6;
uint8_t lostPoints = 0;
uint8_t drawPoints = 3;

// Declare score tracking variable
unsigned long myScore = 0;


// Define structure for holding game plays
typedef struct gamePlay {
    char elfChar;
    uint8_t value;
} gameplay_t;

enum {rockid, paperid, scissorsid};

// Assign the game play hands to throw
gameplay_t ROCK = {'A', rockid+1};
gameplay_t PAPER = {'B', paperid+1};
gameplay_t SCISSORS = {'C', scissorsid+1};

// Add all game play options to an array
gameplay_t * ALL_OPTIONS[3] = {&ROCK, &PAPER, &SCISSORS};

/// @brief Get a gameplay from a character
/// @param symbol The strategy guide character
/// @return The corresponding gameplay symbol
gameplay_t * getGamePlay(char symbol) {
    for (uint8_t index = 0; index < sizeof(ALL_OPTIONS); index++) {
        if (symbol == ALL_OPTIONS[index]->elfChar) {
            return ALL_OPTIONS[index];
        }
    }
}

/// @brief Score the match between the elf and the player
/// @param elfSymbol The hand thrown by the elf
/// @param playerSymbol The hand thrown by the player
/// @return They player's score for the match
uint8_t scoreMatch(gameplay_t *elfSymbol, gameplay_t *playerSymbol) {
    uint8_t matchScore = playerSymbol->value;
    if (elfSymbol == playerSymbol) { return matchScore + drawPoints; }
    if (
        (playerSymbol == &ROCK && elfSymbol == &SCISSORS) ||
        (playerSymbol == &SCISSORS && elfSymbol == &PAPER) ||
        (playerSymbol == &PAPER && elfSymbol == &ROCK)
    ) {
        return matchScore + winPoints;
    }
    return matchScore + lostPoints;
}

/// @brief Get the players hand from the elf's hand and required match result
/// @param elfSymbol The hand the elf will throw
/// @param outcome The character representing the outcome for the match
/// @return The hand the player must throw
gameplay_t * getGamePlayFromOutcome(gameplay_t *elfSymbol, char outcome) {
    
    // Get the players hand index
    int playerHandIndex;
    if (outcome == 'X') {  // Player must lose
        playerHandIndex = elfSymbol->value - 2;
    }
    else if (outcome == 'Y') {  // Player must draw
        playerHandIndex = elfSymbol->value -1;
    }
    else {  // Player must win
        playerHandIndex = elfSymbol->value;
    }

    // Wrap index if necessary
    if (playerHandIndex == -1) {
        playerHandIndex = 2;
    }
    else if (playerHandIndex == 3) {
        playerHandIndex = 0;
    }

    // Get the player's hand
    return ALL_OPTIONS[playerHandIndex];
}


void main(void) {

    // Open the strategy file
    fp = fopen("strategy.txt", "r");

    // Read the file line by line
    while (!feof(fp)) {

        // Initialize variable for characters
        char elfChar, playerChar;
        gameplay_t *elfPlay, *playerPlay;

        // Read a line of the strategy file
        fgets(readBuffer, sizeof(readBuffer), fp);

        // Break if at end of file
        if (feof(fp)) { break; }

        // Parse the string
        sscanf(readBuffer, "%c %c\n", &elfChar, &playerChar);
        elfPlay = getGamePlay(elfChar);
        playerPlay = getGamePlayFromOutcome(elfPlay, playerChar);

        // Calculate match score and add to personal total
        myScore += scoreMatch(elfPlay, playerPlay);
    }

    // Announce personal total
    printf("My total score was %lu\n", myScore);
}
