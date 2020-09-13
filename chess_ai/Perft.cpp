#include "Perft.h"
#include "stdio.h"

void Perft::perft(Board b, int depth) {

	if (depth == 0) {
		counter++;
		return;
	}

	MoveGenerator moveGenerator;
	int numMoves = moveGenerator.generateAllMoves(b);
	Move* moves = moveGenerator.getMoves();

	for (int i = 0; i < numMoves; i++) {
		//getchar();
		cout << "REC Side to play is " << b.side << endl;

		if (!b.push(&b, moves[i].move)) continue;

		cout << "Move " << b.printMove(moves[i].move) << " en pas " << b.enPas << endl;

		perft(b, depth - 1);
		b.pop(&b);

	}

}

void Perft::perftRoot(Board b, int depth) {

	ASSERT(b.checkBoard(&b));

	b.printBoard(&b);
	printf("\nStarting Test To Depth:%d\n", depth);
	counter = 0;

	MoveGenerator moveGenerator;
	int numMoves = moveGenerator.generateAllMoves(b);
	Move* moves = moveGenerator.getMoves();

	cout << "\nnumMoves " << numMoves << "\n";
	moveGenerator.printMoveList(&b);
	cout << "\n\n";


	for (int i = 0; i < numMoves; i++) {
		//getchar();
		cout << "ROOT Side to play is " << b.side << endl;

		if (!b.push(&b, moves[i].move)) continue;

		cout << "\n" << i + 1 << " root branch" << endl;
		cout << "Move " << b.printMove(moves[i].move) << " en pas " << b.enPas << endl;

		long nodes = counter;

		perft(b, depth - 1);
		b.pop(&b);

		long oldnodes = counter - nodes;
		//printf("Move %d : %s : %ld\n", counter, b->printMove(moves[i].move), oldnodes);

	}

	printf("\nTest Complete : %ld nodes visited\n", counter);

	return;

}
