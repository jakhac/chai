#include "tt.h"

void initTT(ttable_t* tt) {
	tt->entries = ttSize / sizeof(ttable_entry_t);
	tt->entries -= 2;
	tt->stored = 0;

	if (tt->table != NULL) {
		free(tt->table);
	}

	// dynamically allocate memory hash table
	tt->table = (ttable_entry_t*)malloc(tt->entries * sizeof(ttable_entry_t));
	clearTT(tt);

	cout << "Transposition table initialized with " << tt->entries << " entries." << endl;
}

void clearTT(ttable_t* tt) {
	ttable_entry_t* ttEntry;
	for (ttEntry = tt->table; ttEntry < tt->table + tt->entries; ttEntry++) {
		ttEntry->zobKey = 0ULL;
		ttEntry->move = 0;
		ttEntry->depth = 0;
		ttEntry->flag = 0;
		ttEntry->score = 0;
	}
}

void storeTT(Board* b, int move, int score, int flag, int depth) {
	int index = b->zobristKey % b->tt->entries;

	Assert(index >= 0 && index <= b->tt->entries - 1);
	Assert(depth >= 1 && depth <= MAX_DEPTH);
	Assert(flag >= TT_ALPHA && flag <= TT_SCORE);
	Assert(score >= -INF && score <= INF);
	Assert(b->ply >= 0 && b->ply <= MAX_DEPTH);

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

bool probeTT(Board* b, int* move, int* score, int alpha, int beta, int depth) {
	int index = b->zobristKey % b->tt->entries;

	Assert(index >= 0 && index <= b->tt->entries - 1);
	Assert(depth >= 1 && depth <= MAX_DEPTH);
	Assert(alpha < beta);
	Assert(alpha >= -INF && alpha <= INF);
	Assert(beta >= -INF && beta <= INF);
	Assert(b->ply >= 0 && b->ply <= MAX_DEPTH);

	if (b->tt->table[index].zobKey == b->zobristKey) {
		*move = b->tt->table[index].move;

		if (b->tt->table[index].depth >= depth) {
			b->tt->valueHit++;

			Assert(b->tt->table[index].depth >= 1 && b->tt->table[index].depth <= MAX_DEPTH);
			Assert(b->tt->table[index].flag >= TT_ALPHA && b->tt->table[index].flag <= TT_SCORE);

			*score = b->tt->table[index].score;

			if (*score > ISMATE) *score -= b->ply;
			else if (*score < -ISMATE) *score += b->ply;

			// if current alpha is lower than stored alpha, no move will increase alpha -> return stored alpha
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
				default: Assert(false) break;
			}
		}
	}

	return false;
}

void prefetchTTEntry(Board* b) {
	int index = b->zobristKey % b->tt->entries;
	_m_prefetch(&b->tt->table[index]);
}

int probePV(Board* b) {
	int index = b->zobristKey % b->tt->entries;
	Assert(index >= 0 && index <= b->tt->entries - 1);

	if (b->tt->table[index].zobKey == b->zobristKey) {
		return b->tt->table[index].move;
	}

	return 0;
}

int getPVLine(Board* b, const int maxDepth) {
	int move = probePV(b);
	int count = 0;

	while (move != 0 && count < maxDepth && isLegal(b, move, b->isCheck(b->side))) {
		b->push(move);
		b->pvArray[count++] = move;
		move = probePV(b);
	}

	while (b->ply > 0) {
		b->pop();
	}
	return count;
}

void initPawnTable(pawntable_t* pawnTable) {
	pawnTable->entries = pawnTableSize / sizeof(pawntable_entry_t);
	pawnTable->entries -= 2;
	pawnTable->stored = 0;

	if (pawnTable->table != NULL) {
		free(pawnTable->table);
	}

	// dynamically allocate memory hash table
	pawnTable->table = (pawntable_entry_t*)malloc(pawnTable->entries * sizeof(pawntable_entry_t));
	clearPawnTable(pawnTable);

	cout << "Pawn table initialized with " << pawnTable->entries << " entries." << endl;
}

void clearPawnTable(pawntable_t* pawnTable) {
	pawntable_entry_t* pawnEntry_s;
	for (pawnEntry_s = pawnTable->table; pawnEntry_s < pawnTable->table + pawnTable->entries; pawnEntry_s++) {
		pawnEntry_s->zobristPawnKey = 0x0;
		pawnEntry_s->eval = 0;
	}

	pawnTable->stored = 0;
	pawnTable->collided = 0;
}

void storePawnEntry(Board* b, const int eval) {
	int index = b->zobristPawnKey % b->pawnTable->entries;
	Assert(index >= 0 && index <= b->pawnTable->entries - 1);

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
	Assert(index >= 0 && index <= b->pawnTable->entries - 1);

	if (b->pawnTable->table[index].zobristPawnKey == b->zobristPawnKey) {
		return b->pawnTable->table[index].eval;
	}

	return NO_SCORE;
}



