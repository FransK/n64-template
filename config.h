
#ifndef N64BASE_CONFIG_H
#define N64BASE_CONFIG_H

#define DEBUG 1

// Initialize USB and isViewer logging
#if defined(DEBUG) && DEBUG == 1
#define DEBUG_LOG 1
#else
#define DEBUG_LOG 0 // Change this one if you just want debugf enabled
#endif

// Skip asking the number of players and assume PLAYER_COUNT
#define SKIP_PLAYERSELECTION 1

// The number of human players
#define PLAYER_COUNT 1

// Skip asking the AI difficulty and assume AI_DIFFICULTY
#define SKIP_DIFFICULTYSELECTION 1

// The difficulty of the AI.
#define AI_DIFFICULTY DIFF_EASY

#endif