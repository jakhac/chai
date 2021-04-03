#include "tt.h"

void initTT(ttable_t* tt) {
	tt->buckets = ttSize / (sizeof(ttable_entry_t) * BUCKETS);
	tt->buckets -= 2;
	tt->stored = 0;

	if (tt->table != NULL) {
		free(tt->table);
	}

	// dynamically allocate memory hash table
	tt->table = (ttable_entry_t*)malloc(tt->buckets * (sizeof(ttable_entry_t) * BUCKETS));
	clearTT(tt);

	cout << "Buckets: " << BUCKETS << endl;
	cout << "Transposition table initialized with " << tt->buckets
		<< " buckets. (" << (tt->buckets * BUCKETS) << " entries)" << endl;

}

void clearTT(ttable_t* tt) {
	memset(tt->table, 0, (tt->buckets * sizeof(ttable_entry_t) * BUCKETS));
}

void storeTT(board_t* b, move_t move, int score, int flag, int depth) {
	int index = (b->zobristKey % b->tt->buckets) * BUCKETS;

	Assert(move != NO_MOVE);
	Assert(index >= 0 && index <= (b->tt->buckets * BUCKETS) - 1);
	Assert(depth >= 1 && depth <= MAX_DEPTH);
	Assert(flag >= TT_ALPHA && flag <= TT_SCORE);
	Assert(score >= -INF && score <= INF);
	Assert(b->ply >= 0 && b->ply <= MAX_DEPTH);

	searchToHash(b, &score);

	// Stats
	b->tt->stored++;

	// compute pointer to current bucket: pTable + (index * buckets)
	ttable_entry_t* bucket = b->tt->table + index;
	int offset = -1;

	// Replacement strategy: new entry overwrites entry with lowest depth
	int minDepth = MAX_DEPTH + 1;
	for (int i = 0; i < BUCKETS; i++) {
		Assert((bucket + i)->depth >= 0 && (bucket + i)->depth <= MAX_DEPTH + 1);

		// Search for lowest depth => longest distance to root and least savings during search
		if ((bucket + i)->depth < minDepth) {
			minDepth = (bucket + i)->depth;
			offset = i;
		}

	}

	Assert(offset >= 0 && offset <= BUCKETS);

	if ((bucket + offset)->flag != TT_NONE) {
		b->tt->collided++;
	}

	if (score > ISMATE) score += b->ply;
	else if (score < -ISMATE) score -= b->ply;

	// Replace entry has been determined: Store information 
	(bucket + offset)->zobKey = b->zobristKey;
	(bucket + offset)->move = move;
	(bucket + offset)->flag = flag;
	(bucket + offset)->score = score;
	(bucket + offset)->depth = depth;
}

bool probeTT(board_t* b, move_t* move, int* hashScore, uint8_t* hashFlag, int* hashDepth) {
	int index = (b->zobristKey % b->tt->buckets) * BUCKETS;

	Assert(index >= 0 && index <= (b->tt->buckets * BUCKETS) - 1);
	Assert(b->ply >= 0 && b->ply <= MAX_DEPTH);

	ttable_entry_t* bucket = b->tt->table + index;
	for (int i = 0; i < BUCKETS; i++) {
		b->tt->probed++;

		if (bucket->zobKey == b->zobristKey) {
			Assert((bucket + i)->flag >= TT_ALPHA && (bucket + i)->flag <= TT_SCORE);
			Assert((bucket + i)->move != NO_MOVE);
			Assert((bucket + i)->score >= -INF && (bucket + i)->score <= INF);

			int newScore = (bucket + i)->score;

			*hashScore = newScore;
			if (*hashScore > ISMATE) *hashScore -= b->ply;
			else if (*hashScore < -ISMATE) *hashScore += b->ply;

			*hashDepth = (bucket + i)->depth;
			*move = (bucket + i)->move;
			*hashFlag = (bucket + i)->flag;
			//*hashScore = (bucket + i)->score;

			return true;
		}
	}

	return false;
}

void prefetchTTEntry(board_t* b) {
	int index = b->zobristKey % b->tt->buckets;
	int entry = index * BUCKETS;

	_m_prefetch(&b->tt->table[entry]);
}

void hashToSearch(board_t* b, int* score) {
	return;

	if (*score > ISMATE) {
		*score -= b->ply;
	} else if (*score < -ISMATE) {
		*score += b->ply;
	}
}

void searchToHash(board_t* b, int* score) {
	return;

	if (*score > ISMATE) {
		score += b->ply;
	} else if (*score < -ISMATE) {
		score -= b->ply;
	}
}

move_t probePV(board_t* b) {
	int index = b->zobristKey % b->tt->buckets;
	int entry = index * BUCKETS;
	Assert(index >= 0 && index <= b->tt->buckets - 1);

	ttable_entry_t* bucket = b->tt->table + entry;

	for (int i = 0; i < BUCKETS; i++) {
		if (bucket->zobKey == b->zobristKey) {
			Assert(bucket->move != NO_MOVE);
			return bucket->move;
		}

		bucket++;
	}

	return NO_MOVE;
}

int getPVLine(board_t* b, const int maxDepth) {
	int move = probePV(b);
	int count = 0;

	while (move != NO_MOVE && count < maxDepth && isLegal(b, move)) {
		push(b, move);
		b->pvArray[count++] = move;
		move = probePV(b);
	}

	while (b->ply > 0) {
		pop(b);
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

void storePawnEntry(board_t* b, const int eval) {
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

void prefetchPawnEntry(board_t* b) {
	int index = b->zobristPawnKey % b->pawnTable->entries;
	_m_prefetch(&b->pawnTable->table[index]);
}

bool probePawnEntry(board_t* b, int* hashScore) {
	int index = b->zobristPawnKey % b->pawnTable->entries;
	Assert(index >= 0 && index <= b->pawnTable->entries - 1);

	if (b->pawnTable->table[index].zobristPawnKey == b->zobristPawnKey) {
		*hashScore = b->pawnTable->table[index].eval;
		return true;
	}

	return false;
}

void destroyTranspositionTables(board_t* b) {
	if (b->tt->table != NULL) {
		free(b->tt->table);
	}
	if (b->pawnTable->table != NULL) {
		free(b->pawnTable->table);
	}
}

void printTTStatus(board_t* b) {

	cout << "Buckets: " << b->tt->buckets << endl;
	cout << "Entries: " << (b->tt->buckets * BUCKETS) << endl;
	cout << "Collided: " << b->tt->collided << "/" << b->tt->stored << endl;
	cout << endl;

	ttable_entry_t* bucket = b->tt->table;

	int firstEntries = 0;
	int secEntries = 0;
	int thirdEntries = 0;
	int fourthEntries = 0;

	for (int i = 0; i < b->tt->buckets; i++) {

		for (int j = 0; j < BUCKETS; j++) {

			// Count non-empty buckets
			if (bucket->zobKey != 0x0) {

				if (bucket->move == NO_MOVE) {

					//cout << "Depth " << (int)bucket->depth << endl;
					//cout << "Score " << (int)bucket->score << endl;
					//cout << "Flag " << (int)bucket->flag << endl;

				}

				Assert(bucket->move != NO_MOVE);

				switch (j) {
					case 0:
						firstEntries++;
						break;
					case 1:
						secEntries++;
						break;
					case 2:
						thirdEntries++;
						break;
					case 3:
						fourthEntries++;
						break;
					default: break;
				}
			}

			bucket++;
		}
	}

	cout << "First entries population: non-empty/buckets " << firstEntries << "/" << b->tt->buckets << endl;
	cout << "Secon entries population: non-empty/buckets " << secEntries << "/" << b->tt->buckets << endl;
	cout << "Third entries population: non-empty/buckets " << thirdEntries << "/" << b->tt->buckets << endl;
	cout << "Fourth entries population: non-empty/buckets " << fourthEntries << "/" << b->tt->buckets << endl;
}
