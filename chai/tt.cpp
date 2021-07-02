#include "tt.h"


void initHashTables(board_t* b) {
	if (!resizeHashTables(b->tt, b->pt, DEFAULT_TT_SIZE)) {
		cerr << "Error in memory allocation for TT." << endl;
		exit(1);
	}
}

void freeHashTables(ttable_t* tt, pawntable_t* pt) {
	if (tt->bucketList != NULL && !VirtualFree(tt->bucketList, 0, MEM_RELEASE)) {
		DWORD err = GetLastError();
		cerr << "Failed to free large page memory. Error code: 0x"
			<< std::hex << err
			<< std::dec << endl;
		exit(1);
	}
	if (pt->table != NULL) {
		free(pt->table);
	}

	pt->table = NULL;
	tt->bucketList = NULL;
}

// allocate tt with virtualAlloc, return unused MB 
static size_t allocateTT(ttable_t* tt, size_t newMbSize) {
	unsigned long long totalBytes = (unsigned long long)newMbSize << 20;
	unsigned long long numBucketsPossible = totalBytes / sizeof(bucket_t);

	// Most significant bit is maximum power of 2 while smaller than number of buckets
	int msb = getMSB(numBucketsPossible);
	Assert(pow(2, msb) <= numBucketsPossible);

	if (msb > 48) {
		cerr << "Error: TT-Size is too big." << endl;
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
static size_t allocatePT(pawntable_t* pt, size_t remainingByte) {
	if (remainingByte < (DEFAULT_PT_SIZE << 20)) {
		remainingByte = DEFAULT_PT_SIZE << 20;
	} else if (remainingByte > (MAX_PT_SIZE << 20)) {
		remainingByte = MAX_PT_SIZE << 20;
	}

	pt->entries = remainingByte / sizeof(pawntable_entry_t);
	pt->entries -= 2;
	pt->stored = 0;

	pt->table = (pawntable_entry_t*)malloc(pt->entries * sizeof(pawntable_entry_t));
	clearPT(pt);

	// return used bytes
	return remainingByte;
}

bool resizeHashTables(ttable_t* tt, pawntable_t* pt, size_t newMbSize) {
	if (newMbSize < MIN_TT_SIZE || newMbSize > MAX_TT_SIZE) {
		cerr << "Request MB is too small / large. Choose from interval ["
			<< MIN_TT_SIZE << ", " << MAX_TT_SIZE << "]" << endl;
		return false;
	}

	freeHashTables(tt, pt);
	Assert(!tt->bucketList);
	Assert(!pt->table);

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

void clearTT(ttable_t* tt) {
	memset(tt->bucketList, 0, (tt->buckets * sizeof(bucket_t)));
}

void clearPT(pawntable_t* pt) {
	memset(pt->table, 0, (pt->entries * sizeof(pawntable_entry_t)));
}

static uint32_t getTTIndex(key_t zobristKey) {
	return (uint32_t)(zobristKey & indexMask);
}

static uint16_t getBucketIndex(key_t zobristKey) {
	return (uint16_t)(zobristKey >> 48);
}

void storeTT(board_t* b, move_t move, value_t value, value_t staticEval, int flag, int depth) {
	int32_t index = getTTIndex(b->zobristKey);
	uint16_t key = getBucketIndex(b->zobristKey);

	Assert(move != MOVE_NONE);
	Assert(flag >= TT_ALPHA && flag <= TT_EVAL);
	Assert(index >= 0 && index <= (b->tt->buckets - 1));
	Assert(depth >= QS_DEPTH && depth <= MAX_DEPTH);
	Assert(flag >= TT_ALPHA && flag <= TT_EVAL);
	Assert(abs(value) < VALUE_INFTY || value == VALUE_NONE);
	Assert(abs(staticEval) < VALUE_INFTY);
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

		Assert(e->depth >= QS_DEPTH && e->depth <= MAX_DEPTH + 1);

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

void storePT(board_t* b, const value_t eval) {
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

bool probeTT(board_t* b, move_t* move, value_t* hashValue, value_t* hashEval, uint8_t* hashFlag, int8_t* hashDepth) {
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
			Assert(e->move != MOVE_NONE);
			Assert(abs(e->value) < VALUE_INFTY || e->value == VALUE_NONE);
			Assert(abs(e->staticEval) < VALUE_IS_MATE_IN);

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

bool probePT(board_t* b, value_t* hashScore) {
	int index = b->zobristPawnKey % b->pt->entries;
	Assert(index >= 0 && index <= b->pt->entries - 1);

	if (b->pt->table[index].zobristPawnKey == b->zobristPawnKey) {
		*hashScore = b->pt->table[index].eval;
		return true;
	}

	return false;
}

void prefetchTT(board_t* b) {
	uint32_t index = getTTIndex(b->zobristKey);
	prefetch((bucket_t*)&b->tt->bucketList[index]);
}

void prefetchPT(board_t* b) {
	int index = b->zobristPawnKey % b->pt->entries;
	prefetch(&b->pt->table[index]);
}

int hashToSearch(board_t* b, value_t score) {
	if (score > VALUE_IS_MATE_IN) {
		return score - b->ply;
	} else if (score < -VALUE_IS_MATE_IN) {
		return score + b->ply;
	}
	return score;
}

int searchToHash(board_t* b, value_t score) {
	if (score > VALUE_IS_MATE_IN) {
		return score + b->ply;
	} else if (score < -VALUE_IS_MATE_IN) {
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

		if (bucket->bucketEntries[i].key == key
			&& bucket->bucketEntries[i].flag != TT_NONE) {
			e = &bucket->bucketEntries[i];

			Assert(e->flag >= TT_ALPHA && e->flag <= TT_EVAL);
			Assert(e->move != MOVE_NONE);
			Assert(e->value >= -VALUE_INFTY && e->value <= VALUE_INFTY);

			return e->move;

		}
	}

	return MOVE_NONE;

}

int getPVLine(board_t* b, const int maxDepth) {
	int move = probePV(b);
	int count = 0;

	while (move != MOVE_NONE && count < maxDepth && isLegal(b, move)) {
		push(b, move);
		b->pvArray[count++] = move;
		move = probePV(b);
	}

	while (b->ply > 0) {
		pop(b);
	}

	return count;
}
