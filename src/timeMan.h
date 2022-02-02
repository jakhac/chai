#include "board.h"

/**
 * @brief Return the material in pawn=1 ratio.
 */
int totalMaterial(board_t* b);

/**
 * @brief Estimate the number of remaining halfmoves.
 */
int remainingHalfMoves(board_t* b);

/**
 * @brief Allocate time for next move.
 */
int allocateTime(board_t* b, int timeLeft, int inc);

/**
 * @brief Returns true if we have more time to search the postition.
 */
bool isTimeLeft(instr_t* instr);