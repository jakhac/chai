#include "perft.h"

long long Perft::perftRoot(Board* b, int depth) {
	ASSERT(b->checkBoard());
	printf("\nPerft to depth %d\n", depth);
	auto start = std::chrono::high_resolution_clock::now();

	leafNodes = 0;
	moveList_t _moveList[1];
	generateMoves(b, _moveList);

	int move;
	int moveNum = 0;
	for (int i = 0; i < _moveList->cnt; i++) {
		move = _moveList->moves[i];

		//skip illegal moves
		if (!b->push(move)) continue;

		long long cumnodes = leafNodes;
		perft(b, depth - 1);
		b->pop();
		long long oldnodes = leafNodes - cumnodes;

		cout << getStringMove(move) << " : " << oldnodes << endl;
	}

	cout << "\nTest Complete : " << leafNodes << " nodes visited" << endl;
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << duration.count() << "ms\n";
	return leafNodes;
}

void Perft::perft(Board* b, int depth) {

	ASSERT(b->checkBoard());

	if (depth == 0) {
		leafNodes++;
		return;
	}

	moveList_t _moveList[1];
	generateMoves(b, _moveList);

	ASSERT(b->attackerSet(b->side ^ 1) == _moveList->attackedSquares);

	int move;
	for (int i = 0; i < _moveList->cnt; i++) {
		move = _moveList->moves[i];

		// skip illegal moves
		if (!b->push(move)) continue;

		perft(b, depth - 1);
		b->pop();
	}
}

long long Perft::perftLegalRoot(Board* b, int depth) {
	ASSERT(b->checkBoard());
	printf("\nPerft to depth %d\n", depth);
	auto start = std::chrono::high_resolution_clock::now();

	leafNodes = 0;
	moveList_t _moveList[1];
	generateMoves(b, _moveList);

	bool inCheck = b->isCheck(b->side);
	int move;
	int moveNum = 0;
	for (int i = 0; i < _moveList->cnt; i++) {
		move = _moveList->moves[i];

		//skip illegal moves
		if (isLegal(b, move, _moveList->attackedSquares, inCheck)) {
			b->push(move);

			long long cumnodes = leafNodes;
			perft(b, depth - 1);

			ASSERT(b->undoPly > 0);
			b->pop();
			long long oldnodes = leafNodes - cumnodes;

			cout << getStringMove(move) << " : " << oldnodes << endl;
		}
	}

	cout << "\nTest Complete : " << leafNodes << " nodes visited" << endl;
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << duration.count() << "ms\n";
	return leafNodes;
}

void Perft::perftLegal(Board* b, int depth) {

	ASSERT(b->checkBoard());

	if (depth == 0) {
		leafNodes++;
		return;
	}

	moveList_t _moveList[1];
	generateMoves(b, _moveList);

	bool inCheck = b->isCheck(b->side);
	ASSERT(b->attackerSet(b->side ^ 1) == _moveList->attackedSquares);

	int move;
	for (int i = 0; i < _moveList->cnt; i++) {
		move = _moveList->moves[i];

		//skip illegal moves
		if (isLegal(b, move, _moveList->attackedSquares, inCheck)) {
			b->push(move);
			ASSERT(!b->isCheck(b->side ^ 1));

			perftLegal(b, depth - 1);

			ASSERT(b->undoPly > 0);
			b->pop();
		}
	}
}

long long Perft::perftBulkRoot(Board* b, int depth) {
	ASSERT(b->checkBoard());
	printf("\nPerft to depth %d\n", depth);
	auto start = std::chrono::high_resolution_clock::now();

	leafNodes = 0;
	moveList_t _moveList[1];
	_generateMoves(b, _moveList);

	int move;
	int moveNum = 0;
	for (int i = 0; i < _moveList->cnt; i++) {
		move = _moveList->moves[i];

		b->push(move);

		long long cumnodes = leafNodes;
		leafNodes += perftBulk(b, depth - 1);
		b->pop();
		long long oldnodes = leafNodes - cumnodes;

		cout << getStringMove(move) << ": " << oldnodes << endl;
	}

	cout << "\nTest Complete : " << leafNodes << " nodes visited" << endl;
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << duration.count() << "ms\n";
	return leafNodes;
}


long long Perft::perftBulk(Board* b, int depth) {

	long long nodes = 0;

	moveList_t move_s[1];
	_generateMoves(b, move_s);

	if (depth == 1)
		return move_s->cnt;

	for (int i = 0; i < move_s->cnt; i++) {
		b->push(move_s->moves[i]);
		nodes += perftBulk(b, depth - 1);
		b->pop();
	}

	return nodes;
}