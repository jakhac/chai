#include "tt.h"

void initTT(TT_S* tt) {
	tt->entries = ttSize / sizeof(TT_ENTRY_S);
	tt->entries -= 2;
	tt->stored = 0;

	if (tt->table != NULL) {
		free(tt->table);
	}

	// dynamically allocate memory hash table
	tt->table = (TT_ENTRY_S*)malloc(tt->entries * sizeof(TT_ENTRY_S));
	clearTT(tt);

	cout << "Transposition table initialized with " << tt->entries << " entries." << endl;
}

void clearTT(TT_S* tt) {
	TT_ENTRY_S* ttEntry;
	for (ttEntry = tt->table; ttEntry < tt->table + tt->entries; ttEntry++) {
		ttEntry->zobKey = 0ULL;
		ttEntry->move = 0;
		ttEntry->depth = 0;
		ttEntry->flag = 0;
		ttEntry->score = 0;
	}
}

// store a move into pvtable of board
void storeTT(Board* b, int move, int score, int flag, int depth) {
	int index = b->zobristKey % b->tt->entries;

	ASSERT(index >= 0 && index <= b->tt->entries - 1);
	ASSERT(depth >= 1 && depth <= MAX_DEPTH);
	ASSERT(flag >= TT_ALPHA && flag <= TT_SCORE);
	ASSERT(score >= -INF && score <= INF);
	ASSERT(b->ply >= 0 && b->ply <= MAX_DEPTH);

	if (score > ISMATE) score += b->ply;
	else if (score < -ISMATE) score -= b->ply;

	// stats
	if (b->tt->table[index].zobKey != 0ULL) {
		b->tt->collided++;
	} else {
		b->tt->stored++;
	}

	b->tt->table[index].move = move;
	b->tt->table[index].zobKey = b->zobristKey;
	b->tt->table[index].flag = flag;
	b->tt->table[index].score = score;
	b->tt->table[index].depth = depth;
}

int probeTT(Board* b, int* move, int* score, int alpha, int beta, int depth) {
	int index = b->zobristKey % b->tt->entries;

	ASSERT(index >= 0 && index <= b->tt->entries - 1);
	ASSERT(depth >= 1 && depth <= MAX_DEPTH);
	ASSERT(alpha < beta);
	ASSERT(alpha >= -INF && alpha <= INF);
	ASSERT(beta >= -INF && beta <= INF);
	ASSERT(b->ply >= 0 && b->ply <= MAX_DEPTH);

	if (b->tt->table[index].zobKey == b->zobristKey) {
		*move = b->tt->table[index].move;
		
		if (b->tt->table[index].depth >= depth) {
			b->tt->valueHit++;

			ASSERT(b->tt->table[index].depth >= 1 && b->tt->table[index].depth <= MAX_DEPTH);
			ASSERT(b->tt->table[index].flag >= TT_ALPHA && b->tt->table[index].flag <= TT_SCORE);

			*score = b->tt->table[index].score;
			
			if (*score > ISMATE) *score -= b->ply;
			else if (*score < -ISMATE) *score += b->ply;

			// try to overwrite score reference, if possible return true
			switch (b->tt->table[index].flag) {
				case TT_ALPHA:
					if (*score <= alpha) {
						*score = alpha;
						return true;
					}
					break;
				case TT_BETA:
					if (*score >= beta) {
						*score = beta;
						return true;
					}
					break;
				case TT_SCORE:
					return true;
					break;
				default: ASSERT(false) break;
			}
		}
	}

	return false;
}

int probePV(Board* b) {
	int index = b->zobristKey % b->tt->entries;
	ASSERT(index >= 0 && index <= b->tt->entries - 1);

	if (b->tt->table[index].zobKey == b->zobristKey) {
		return b->tt->table[index].move;
	}
	
	return 0;
}

int getPVLine(Board* b, const int maxDepth) {
	int move = probePV(b);
	int count = 0;

	while (move != 0 && count < maxDepth && moveLegal(b, move)) {
		b->push(move);
		b->pvArray[count++] = move;
		move = probePV(b);
	}

	while (b->ply > 0) {
		b->pop();
	}

	return count;
}


void initPawnTable(PAWN_TABLE_S* pawnTable) {
	pawnTable->entries = pawnTableSize / sizeof(PAWN_ENTRY_S);
	pawnTable->entries -= 2;
	pawnTable->stored = 0;

	if (pawnTable->table != NULL) {
		free(pawnTable->table);
	}

	// dynamically allocate memory hash table
	pawnTable->table = (PAWN_ENTRY_S*)malloc(pawnTable->entries * sizeof(PAWN_ENTRY_S));
	clearPawnTable(pawnTable);

	cout << "Pawn table initialized with " << pawnTable->entries << " entries." << endl;
}

void clearPawnTable(PAWN_TABLE_S* pawnTable) {
	PAWN_ENTRY_S* pawnEntry_s;
	for (pawnEntry_s = pawnTable->table; pawnEntry_s < pawnTable->table + pawnTable->entries; pawnEntry_s++) {
		pawnEntry_s->zobristPawnKey = 0x0;
		pawnEntry_s->eval = 0;
	}

	pawnTable->stored = 0;
	pawnTable->collided = 0;
}

void storePawnEntry(Board* b, const int eval) {
	int index = b->zobristPawnKey % b->pawnTable->entries;
	ASSERT(index >= 0 && index <= b->pawnTable->entries - 1);

	if (b->pawnTable->table[index].zobristPawnKey == 0ULL) {
		// count every new entry
		b->pawnTable->stored++;
	} else {
		b->pawnTable->collided++;
	}

	b->pawnTable->table[index].eval = eval;
	b->pawnTable->table[index].zobristPawnKey = b->zobristPawnKey;
}

int probePawnEntry(Board* b) {
	int index = b->zobristPawnKey % b->pawnTable->entries;
	ASSERT(index >= 0 && index <= b->pawnTable->entries - 1);

	if (b->pawnTable->table[index].zobristPawnKey == b->zobristPawnKey) {
		return b->pawnTable->table[index].eval;
	}

	return NO_SCORE;
}



