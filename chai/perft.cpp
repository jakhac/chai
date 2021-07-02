#include "perft.h"

long long Perft::perftRoot(board_t* b, int depth) {
	Assert(checkBoard(b));
	printf("\nPerft to depth %d\n", depth);
	auto start = std::chrono::high_resolution_clock::now();

	leafNodes = 0;
	bool inCheck = isCheck(b, b->stm);
	moveList_t _moveList[1];
	generateMoves(b, _moveList, inCheck);

	int move;
	int moveNum = 0;
	for (int i = 0; i < _moveList->cnt; i++) {
		move = _moveList->moves[i];

#ifdef ASSERT
		bool moveGivesCheck = checkingMove(b, move);
#endif // ASSERT

		//skip illegal moves
		if (!push(b, move)) {
			Assert(!inCheck);
			continue;
		}

		Assert(isCheck(b, b->stm) == moveGivesCheck);

		long long cumnodes = leafNodes;
		perft(b, depth - 1);
		pop(b);
		long long oldnodes = leafNodes - cumnodes;

		cout << getStringMove(b, move) << " : " << oldnodes << endl;
	}

	cout << "\nTest Complete : " << leafNodes << " nodes visited" << endl;
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	cout << duration.count() << "ms\n";
	return leafNodes;
}

void Perft::perft(board_t* b, int depth) {
	Assert(checkBoard(b));

	if (depth == 0) {
		leafNodes++;
		return;
	}

	bool inCheck = isCheck(b, b->stm);
	moveList_t _moveList[1];
	generateMoves(b, _moveList, inCheck);

	Assert(attackerSet(b, b->stm ^ 1) == _moveList->attackedSquares);

	int move;
	for (int i = 0; i < _moveList->cnt; i++) {
		move = _moveList->moves[i];

#ifdef ASSERT
		bool moveGivesCheck = checkingMove(b, move);
#endif // ASSERT

		//skip illegal moves
		if (!push(b, move)) {
			Assert(!inCheck);
			continue;
	}

		Assert(isCheck(b, b->stm) == moveGivesCheck);

		perft(b, depth - 1);
		pop(b);
}
}
