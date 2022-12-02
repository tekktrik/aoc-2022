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
    char playerChar;
    uint8_t value;
} gameplay_t;

enum {rockid, paperid, scissorsid};

// Assign the game play hands to throw
gameplay_t ROCK = {'A', 'X', rockid+1};
gameplay_t PAPER = {'B', 'Y', paperid+1};
gameplay_t SCISSORS = {'C', 'Z', scissorsid+1};

// Add all game play options to an array
gameplay_t * ALL_OPTIONS[3] = {&ROCK, &PAPER, &SCISSORS};

/// @brief Get a gameplay from a character
/// @param symbol The strategy guide character
/// @param isElf Whether the symbol is for the elf of the player
/// @return The corresponding gameplay symbol
gameplay_t * getGamePlay(char symbol, bool isElf) {
    for (uint8_t index = 0; index < sizeof(ALL_OPTIONS); index++) {
        bool isSymbol = isElf ? symbol == ALL_OPTIONS[index]->elfChar : symbol == ALL_OPTIONS[index]->playerChar;
        if (isSymbol) {
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
        elfPlay = getGamePlay(elfChar, true);
        playerPlay = getGamePlay(playerChar, false);

        // Calculate match score and add to personal total
        myScore += scoreMatch(elfPlay, playerPlay);
    }

    // Announce personal total
    printf("My total score was %lu\n", myScore);
}
