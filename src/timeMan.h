#include "board.h"

/**
 * @brief Return the material in pawn=1 ratio.
 */
int totalMaterial(Board* b);

/**
 * @brief Estimate the number of remaining halfmoves.
 */
int remainingHalfMoves(Board* b);

/**
 * @brief Allocate time for next move.
 */
int allocateTime(Board* b, int timeLeft, int inc);

/**
 * @brief Returns true if we have more time to search the postition.
 */
bool isTimeLeft(Instructions* instr);