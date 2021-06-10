#include "tt.h"

// allocate tt with virtualAlloc, return unused MB 
size_t allocateTT(ttable_t* tt, size_t newMbSize) {
	unsigned long long totalBytes = (unsigned long long)newMbSize << 20;
	unsigned long long numBucketsPossible = totalBytes / sizeof(bucket_t);

	// Most significant bit is maximum power of 2 while smaller than number of buckets
	int msb = bitscanReverse(numBucketsPossible);
	Assert(pow(2, msb) <= numBucketsPossible);

	if (msb > 48) {
		cerr << "Error: TT-Size is too big (cannot use lower 48-bits anymore to index table)." << endl;
		exit(1);
	}

	tt->buckets = 1 << msb;
	tt->bucketList = (bucket_t*)VirtualAlloc(NULL, sizeof(bucket_t) * tt->buckets, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	Assert(tt->bucketList);

	for (int i = 0; i < msb; i++) indexMask |= (1 << i);
	clearTT(tt);

	// return remaining bytes
	return totalBytes - (sizeof(bucket_t) * tt->buckets);
}

// Init pawn table with clamped remaining MBs
size_t allocatePT(pawntable_t* pt, size_t remainingByte) {
	if (remainingByte < (DEFAULT_PT_SIZE << 20)) {
		remainingByte = DEFAULT_PT_SIZE << 20;
	} else if (remainingByte > (MAX_PT_SIZE << 20)) {
		remainingByte = MAX_PT_SIZE << 20;
	}

	pt->entries = remainingByte / sizeof(pawntable_entry_t);
	pt->entries -= 2;
	pt->stored = 0;

	pt->table = (pawntable_entry_t*)malloc(pt->entries * sizeof(pawntable_entry_t));
	clearPawnTable(pt);

	// return used bytes
	return remainingByte;
}

// try to resize tt, returns true if successfull, else false
bool resizeTT(ttable_t* tt, pawntable_t* pt, size_t newMbSize) {
	if (newMbSize < MIN_TT_SIZE || newMbSize > MAX_TT_SIZE) {
		cout << "Request MB is too small / large. Choose from interval ["
			<< MIN_TT_SIZE << ", " << MAX_TT_SIZE << "]" << endl;
		return false;
	}

	freeTT(tt, pt);

	size_t possibleBytes = (newMbSize << 20);
	size_t remainingBytes = allocateTT(tt, newMbSize);
	size_t usedBytes = allocatePT(pt, remainingBytes);

#ifdef INFO
	if ((newMbSize & (newMbSize - 1)) != 0) {
		cout << "TT only allocates MB sizes if power of 2. "
			<< newMbSize << "MB is adjusted to "
			<< ((possibleBytes - remainingBytes) >> 20) << "MB"
			<< endl << endl;
	}

	cout
		<< "TT " << ((possibleBytes - remainingBytes) >> 20) << "MB"
		<< " initialized with " << tt->buckets
		<< " buckets. (" << (tt->buckets * BUCKETS) << " entries)" << endl
		<< "PT " << (usedBytes >> 20) << "MB"
		<< " initialized with " << pt->entries << " entries." << endl;
#endif // !INFO

	return true;
}

void freeTT(ttable_t* tt, pawntable_t* pt) {
	if (tt->bucketList != NULL && !VirtualFree(tt->bucketList, 0, MEM_RELEASE)) {
		DWORD err = GetLastError();
		cerr << "Failed to free large page memory. Error code: 0x"
			<< hex << err
			<< dec << endl;
		exit(1);
	}
	if (pt->table != NULL) {
		free(pt->table);
	}

	pt->table = NULL;
	tt->bucketList = NULL;
}

void clearTT(ttable_t* tt) {
	memset(tt->bucketList, 0, (tt->buckets * sizeof(bucket_t)));
}

uint32_t getTTIndex(key_t zobristKey) {
	return (uint32_t)(zobristKey & indexMask);
}

uint16_t getBucketIndex(key_t zobristKey) {
	return (uint16_t)(zobristKey >> 48);
}

void storeTT(board_t* b, move_t move, value_t value, value_t staticEval, int flag, int depth) {
	int32_t index = getTTIndex(b->zobristKey);
	uint16_t key = getBucketIndex(b->zobristKey);

	Assert(move != NO_MOVE);
	Assert(flag >= TT_ALPHA && flag <= TT_EVAL);
	Assert(index >= 0 && index <= (b->tt->buckets - 1));
	Assert((depth >= 1 || TT_EVAL) && depth <= MAX_DEPTH);
	Assert(flag >= TT_ALPHA && flag <= TT_EVAL);
	Assert(abs(value) < INF || value == NO_VALUE);
	Assert(abs(staticEval) < INF);
	Assert(b->ply >= 0 && b->ply <= MAX_DEPTH);

	// Stats
	b->tt->stored++;

	// Iterate entries in bucket and find least valuable entry
	bucket_t* bucket = b->tt->bucketList + index;
	ttable_entry_t* e = nullptr;
	ttable_entry_t* leastValuable = nullptr;

	int minDepth = MAX_DEPTH + 1;
	for (int i = 0; i < BUCKETS; i++) {
		e = bucket->bucketEntries + i;

		Assert(e->depth >= 0 && e->depth <= MAX_DEPTH + 1);

		// Empty entry
		if (e->flag & TT_NONE) {
			Assert(e->depth == 0);
			Assert(e->move == 0);

			leastValuable = e;
			break;
		}

		if (e->key == key) {
			if (depth >= e->depth || flag & TT_VALUE) {
				leastValuable = e;
				break;
			} else {
				return;
			}
		}

		// Minimum Depth (longest distance to root thus least savings during search)
		if (e->depth < minDepth) {
			leastValuable = e;
			minDepth = e->depth;
		}
	}

	Assert(leastValuable);
	if (leastValuable->flag != TT_NONE) {
		b->tt->collided++;
	}

	// Replace entry has been determined: Store information 
	leastValuable->key = key;
	leastValuable->move = move;
	leastValuable->flag = flag;
	leastValuable->value = value;
	leastValuable->staticEval = staticEval;
	leastValuable->depth = depth;
}

bool probeTT(board_t* b, move_t* move, value_t* hashValue, value_t* hashEval, uint8_t* hashFlag, int* hashDepth) {
	int32_t index = getTTIndex(b->zobristKey);
	uint16_t key = getBucketIndex(b->zobristKey);

	Assert(index >= 0 && index <= (b->tt->buckets - 1));
	Assert(b->ply >= 0 && b->ply <= MAX_DEPTH);

	bucket_t* bucket = b->tt->bucketList + index;
	ttable_entry_t* e;

	for (int i = 0; i < BUCKETS; i++) {
		b->tt->probed++;

		if (bucket->bucketEntries[i].key == key
			&& bucket->bucketEntries[i].flag != TT_NONE) {

			e = &bucket->bucketEntries[i];
			Assert(e->flag >= TT_ALPHA && e->flag <= TT_EVAL);
			Assert(e->move != NO_MOVE);
			Assert(abs(e->value) < INF || e->value == NO_VALUE);
			Assert(abs(e->staticEval) < ISMATE);

			*hashValue = hashToSearch(b, e->value);
			*hashDepth = e->depth;
			*move = e->move;
			*hashFlag = e->flag;
			*hashEval = e->staticEval;
			return true;

		}
	}

	return false;
}

void prefetchTTEntry(board_t* b) {

	uint32_t index = getTTIndex(b->zobristKey);
#ifdef __GNUC__
	__builtin_prefetch((bucket_t*)&b->tt->bucketList[index]);
#else
	_m_prefetch((bucket_t*)&b->tt->bucketList[index]);
#endif
}

int hashToSearch(board_t* b, value_t score) {
	if (score > ISMATE) {
		return score - b->ply;
	} else if (score < -ISMATE) {
		return score + b->ply;
	}
	return score;
}

int searchToHash(board_t* b, value_t score) {
	if (score > ISMATE) {
		return score + b->ply;
	} else if (score < -ISMATE) {
		return score - b->ply;
	}
	return score;
}

move_t probePV(board_t* b) {
	int32_t index = getTTIndex(b->zobristKey);
	uint16_t key = getBucketIndex(b->zobristKey);
	Assert(index == int32_t(b->zobristKey & indexMask));

	Assert(index >= 0 && index <= (b->tt->buckets - 1));
	Assert(b->ply >= 0 && b->ply <= MAX_DEPTH);

	bucket_t* bucket = b->tt->bucketList + index;
	ttable_entry_t* e;

	for (int i = 0; i < BUCKETS; i++) {
		b->tt->probed++;

		if (bucket->bucketEntries[i].key == key) {
			e = &bucket->bucketEntries[i];
			Assert(e->flag >= TT_ALPHA && e->flag <= TT_EVAL);
			Assert(e->move != NO_MOVE);
			Assert(e->value >= -INF && e->value <= INF);

			return e->move;

		}
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

void clearPawnTable(pawntable_t* pawnTable) {
	pawntable_entry_t* pawnEntry_s;
	for (pawnEntry_s = pawnTable->table; pawnEntry_s < pawnTable->table + pawnTable->entries; pawnEntry_s++) {
		pawnEntry_s->zobristPawnKey = 0x0;
		pawnEntry_s->eval = 0;
	}

	pawnTable->stored = 0;
	pawnTable->collided = 0;
}

void storePawnEntry(board_t* b, const value_t eval) {
	int index = b->zobristPawnKey % b->pt->entries;
	Assert(index >= 0 && index <= b->pt->entries - 1);

	if (b->pt->table[index].zobristPawnKey == 0ULL) {
		// count every new entry
		b->pt->stored++;
	} else {
		b->pt->collided++;
	}

	b->pt->table[index].eval = eval;
	b->pt->table[index].zobristPawnKey = b->zobristPawnKey;
}

void prefetchPawnEntry(board_t* b) {
	int index = b->zobristPawnKey % b->pt->entries;
#ifdef __GNUC__
	__builtin_prefetch(&b->pt->table[index]);
#else
	_m_prefetch(&b->pt->table[index]);
#endif
}

bool probePawnEntry(board_t* b, value_t* hashScore) {
	int index = b->zobristPawnKey % b->pt->entries;
	Assert(index >= 0 && index <= b->pt->entries - 1);

	if (b->pt->table[index].zobristPawnKey == b->zobristPawnKey) {
		*hashScore = b->pt->table[index].eval;
		return true;
	}

	return false;
}

void printTTStatus(board_t* b) {

	cout << "Buckets: " << b->tt->buckets << endl;
	cout << "Entries: " << (b->tt->buckets * BUCKETS) << endl;
	cout << "Collided: " << b->tt->collided << "/" << b->tt->stored << endl;
	cout << endl;

	bucket_t* bucket = b->tt->bucketList;

	int firstEntries = 0;
	int secEntries = 0;
	int thirdEntries = 0;
	int fourthEntries = 0;

	for (int i = 0; i < b->tt->buckets; i++) {

		for (int j = 0; j < BUCKETS; j++) {

			// Count non-empty buckets
			if (bucket->bucketEntries->key != 0x0) {

				if (bucket->bucketEntries->move == NO_MOVE) {

					//cout << "Depth " << (int)bucket->depth << endl;
					//cout << "Score " << (int)bucket->score << endl;
					//cout << "Flag " << (int)bucket->flag << endl;

				}

				Assert(bucket->bucketEntries->move != NO_MOVE);

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
