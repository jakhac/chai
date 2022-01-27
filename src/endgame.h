#pragma once

#include "board.h"

namespace chai {
    
// Endgames:
//      - KvKNB: force weak king to corner with same color of bishop
//      - KQvKR: force weak king to border/corner of board
//      - KvKX: force weak king to border/corner of board
//      - KQvKP: detect draw on 7-th rank


const int cornerReward[2][64] = {
    { // BLACK
        0, 1, 2, 3, 4, 5, 6, 7, 
        1, 2, 3, 4, 5, 6, 7, 6,
        2, 3, 4, 5, 6, 7, 6, 5,
        3, 4, 5, 6, 7, 6, 5, 4,
        4, 5, 6, 7, 6, 5, 4, 3,
        5, 6, 7, 6, 5, 4, 3, 2,
        6, 7, 6, 5, 4, 3, 2, 1,
        7, 6, 5, 4, 3, 2, 1, 0
    },
    { // WHITE
        7, 6, 5, 4, 3, 2, 1, 0,
        6, 7, 6, 5, 4, 3, 2, 1,
        5, 6, 7, 6, 5, 4, 3, 2,
        4, 5, 6, 7, 6, 5, 4, 3,
        3, 4, 5, 6, 7, 6, 5, 4,
        2, 3, 4, 5, 6, 7, 6, 5,
        1, 2, 3, 4, 5, 6, 7, 6,
        0, 1, 2, 3, 4, 5, 6, 7
    }, 
};

const int borderReward[64] = {
    9, 8, 8, 8, 8, 8, 8, 9, 
    8, 7, 4, 4, 4, 4, 7, 8,
    8, 6, 3, 1, 1, 3, 6, 8,
    8, 5, 1, 0, 0, 1, 5, 8,
    8, 5, 1, 0, 0, 1, 5, 8,
    8, 6, 3, 1, 1, 3, 6, 8,
    8, 7, 6, 5, 5, 6, 7, 8,
    9, 8, 8, 8, 8, 8, 8, 9
};



bool isKvKNB(board_t* b);
bool isKQvKR(board_t* b);
bool isKvKX(board_t* b);
bool isKQvKP(board_t* b);
bool isKQvKQ(board_t* b);

value_t evaluate_KvKNB(board_t* b);
value_t evaluate_KQvKR(board_t* b);
value_t evaluate_KvKX(board_t* b);
value_t evaluate_KQvKP(board_t* b);
value_t evaluate_KQvKQ(board_t* b);


} // namespace chai
