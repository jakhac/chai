#include "perft.h"

long long Perft::perftRoot(board_t* b, int depth) {
	Assert(checkBoard(b));
	printf("\nPerft to depth %d\n", depth);
	auto start = std::chrono::high_resolution_clock::now();

	leafNodes = 0;
	bool inCheck = isCheck(b, b->side);
	moveList_t _moveList[1];
	generateMoves(b, _moveList, inCheck);

	int move;
	int moveNum = 0;
	for (int i = 0; i < _moveList->cnt; i++) {
		move = _moveList->moves[i];

		bool moveGivesCheck = checkingMove(b, move);

		//skip illegal moves
		if (!push(b, move)) {
			Assert(!inCheck);
			continue;
		}

		if (isCheck(b, b->side) != moveGivesCheck) {
			cout << "error in moveGivesCheck" << endl;
			exit(1);
		}
		Assert(isCheck(b, b->side) == moveGivesCheck);

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

	bool inCheck = isCheck(b, b->side);
	moveList_t _moveList[1];
	generateMoves(b, _moveList, inCheck);

	Assert(attackerSet(b, b->side ^ 1) == _moveList->attackedSquares);

	int move;
	for (int i = 0; i < _moveList->cnt; i++) {
		move = _moveList->moves[i];

		bool moveGivesCheck = checkingMove(b, move);

		//skip illegal moves
		if (!push(b, move)) {
			Assert(!inCheck);
			continue;
		}

		if (isCheck(b, b->side) != moveGivesCheck) {
			cout << "error in moveGivesCheck" << endl;
			pop(b);
			printBoard(b);
			cout << "moveGivesCheck " << moveGivesCheck << endl;
			cout << "move " << getStringMove(b, move) << endl;
			cout << "fen " << getFEN(b) << endl;
			exit(1);
		}
		Assert(isCheck(b, b->side) == moveGivesCheck);

		perft(b, depth - 1);
		pop(b);
	}
}

//long long Perft::perftLegalRoot(board_t* b, int depth) {
//	Assert(checkBoard(b));
//	printf("\nPerft to depth %d\n", depth);
//	auto start = std::chrono::high_resolution_clock::now();
//
//	leafNodes = 0;
//	moveList_t _moveList[1];
//	generateMoves(b, _moveList, isCheck(b, b->side));
//
//	bool inCheck = isCheck(b, b->side);
//	int move;
//	int moveNum = 0;
//	for (int i = 0; i < _moveList->cnt; i++) {
//		move = _moveList->moves[i];
//
//		//skip illegal moves
//		if (isLegal(b, move, inCheck)) {
//			b->push(move);
//
//			long long cumnodes = leafNodes;
//			perft(b, depth - 1);
//
//			Assert(b->undoPly > 0);
//			b->pop();
//			long long oldnodes = leafNodes - cumnodes;
//
//			cout << getStringMove(move) << " : " << oldnodes << endl;
//		}
//	}
//
//	cout << "\nTest Complete : " << leafNodes << " nodes visited" << endl;
//	auto stop = std::chrono::high_resolution_clock::now();
//	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
//	cout << duration.count() << "ms\n";
//	return leafNodes;
//}
//
//void Perft::perftLegal(board_t* b, int depth) {
//
//	Assert(checkBoard(b));
//
//	if (depth == 0) {
//		leafNodes++;
//		return;
//	}
//
//	moveList_t _moveList[1];
//	generateMoves(b, _moveList, isCheck(b, b->side));
//
//	bool inCheck = isCheck(b, b->side);
//	Assert(b->attackerSet(b->side ^ 1) == _moveList->attackedSquares);
//
//	int move;
//	for (int i = 0; i < _moveList->cnt; i++) {
//		move = _moveList->moves[i];
//
//		//skip illegal moves
//		if (isLegal(b, move, inCheck)) {
//			b->push(move);
//			Assert(!isCheck(b, b->side ^ 1));
//
//			perftLegal(b, depth - 1);
//
//			Assert(b->undoPly > 0);
//			b->pop();
//		}
//	}
//}

//long long Perft::perftBulkRoot(board_t* b, int depth) {
//	ASSERT(checkBoard(b));
//	printf("\nPerft to depth %d\n", depth);
//	auto start = std::chrono::high_resolution_clock::now();
//
//	leafNodes = 0;
//	moveList_t _moveList[1];
//	_generateMoves(b, _moveList);
//
//	int move;
//	int moveNum = 0;
//	for (int i = 0; i < _moveList->cnt; i++) {
//		move = _moveList->moves[i];
//
//		b->push(move);
//
//		long long cumnodes = leafNodes;
//		leafNodes += perftBulk(b, depth - 1);
//		b->pop();
//		long long oldnodes = leafNodes - cumnodes;
//
//		cout << getStringMove(move) << ": " << oldnodes << endl;
//	}
//
//	cout << "\nTest Complete : " << leafNodes << " nodes visited" << endl;
//	auto stop = std::chrono::high_resolution_clock::now();
//	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
//	cout << duration.count() << "ms\n";
//	return leafNodes;
//}
//
//
//long long Perft::perftBulk(board_t* b, int depth) {
//
//	long long nodes = 0;
//
//	moveList_t move_s[1];
//	_generateMoves(b, move_s);
//
//	if (depth == 1)
//		return move_s->cnt;
//
//	for (int i = 0; i < move_s->cnt; i++) {
//		b->push(move_s->moves[i]);
//		nodes += perftBulk(b, depth - 1);
//		b->pop();
//	}
//
//	return nodes;
//}