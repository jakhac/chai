#include "tt.h"

void initPVTable(PV_TABLE_S* pvTable_s) {
	pvTable_s->entries = pvSize / sizeof(PV_ENTRY_S);
	pvTable_s->entries -= 2;

	if (pvTable_s->pvTable != NULL) {
		free(pvTable_s->pvTable);
	}

	// dynamically allocate memory hash table
	pvTable_s->pvTable = (PV_ENTRY_S*)malloc(pvTable_s->entries * sizeof(PV_ENTRY_S));
	clearPVTable(pvTable_s);

	cout << "PV table initialized with " << pvTable_s->entries << " entries." << endl;
}

void clearPVTable(PV_TABLE_S* pvTable_s) {
	PV_ENTRY_S* pvEntry;
	for (pvEntry = pvTable_s->pvTable; pvEntry < pvTable_s->pvTable + pvTable_s->entries; pvEntry++) {
		pvEntry->zobKey = 0ULL;
		pvEntry->move = 0;
	}
}

// store a move into pvtable of board
void storePV(Board* b, const int move) {
	int index = b->zobristKey % b->pvTable_s->entries;
	ASSERT(index >= 0 && index <= b->pvTable_s->entries - 1);

	b->pvTable_s->pvTable[index].move = move;
	b->pvTable_s->pvTable[index].zobKey = b->zobristKey;
}

int probePV(Board* b) {
	int index = b->zobristKey % b->pvTable_s->entries;
	ASSERT(index >= 0 && index <= b->pvTable_s->entries - 1);

	if (b->pvTable_s->pvTable[index].zobKey = b->zobristKey) {
		return b->pvTable_s->pvTable[index].move;
	}

	return 0;
}

int getPVLine(Board* b, const int maxDepth) {

	int move = probePV(b);
	int count = 0;

	while (move != 0 && count < maxDepth) {
		if (moveLegal(b, move)) {
			b->push(move);
			b->pvArray[count++] = move;
		} else {
			// break if pv line contains non legal move
			break;
		}
		move = probePV(b);
	}

	while (b->ply > 0) {
		b->pop();
	}

	return count;
}