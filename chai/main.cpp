#include "main.h"

/*
* - Evaluation improvements: 
*		King Safety
*		Pawn Structure
*		Mobility
* 
* - Check evasion in quiesence?
* - Late move _pruning_
* - Try pv move before move generation
* - Refactor eval.cpp
* - Move Ordering, extra bonus for promotions
* - code documentation
*/

/* 
* 500 Games vs TSCP181 with 40/0.4
* 
* 275-162-63 (61%) +78
*	- pvs
*	- lmr
*	- futility pruning
*	- static null move pruning
*	- pv move in quiesence
* 
* 285-133-82 (65%) +108
*	- pvs
*	- lmr
*	- futility pruning
*	- static null move pruning
*	+ pv move in quiesence
*
* 312-108-80 (70%) +147
*	- pvs
*	- lmr
*	+ futility pruning
*	- static null move pruning
*	+ pv move in quiesence
* 
* 353-112-70 (71%) +156 [310-119-71 (70%) +147]
*	- pvs
*	- lmr
*	+ futility pruning
*	+ static null move pruning
*	+ pv move in quiesence
*/


int main() {

	Board b;
	Perft perft;
	MOVE_S move_s[1];
	SEARCH_S s[1];

	init(&b);
	b.parseFen(BUG_FEN);
	//b.parseFen("8/7K/8/8/8/8/R7/7k w - - 0 1 ");
	b.printBoard();
	log("\nStartup");

	//dividePerft(&b, BUG_FEN, 5); // 8312, 8031

	play(&b, &perft, s);

	if (b.tt->table != NULL) free(b.tt->table);
	if (b.pawnTable->table != NULL) free(b.pawnTable->table);
	return 0;
}

void play(Board* b, Perft* p, SEARCH_S* s) {
	string m;

	while (true) {
		cout << "\n##################\n\n";
		getline(cin, m);

		log(m);

		// POP MOVE FROM BOARD
		if (m == "pop") {
			UNDO_S pop = b->pop();
			cout << "Popped " << getStringMove(pop.move) << " from stack." << endl;
			b->printBoard();
			continue;
		}

		if (m == "autoplay") {
			while (true) {
				s->depth = 7;
				s->startTime = getTimeMs();
				s->stopTime = getTimeMs() + 10000;

				s->timeSet = false;
				s->depthSet = true;
				search(b, s);

				getPVLine(b, s->depth);

				MOVE_S move_s[1];
				generateMoves(b, move_s);
				b->push(move_s->moveList[move_s->moveCounter/3]);
				cout << "Push move " << b->halfMoves << ". " << getStringMove(move_s->moveList[move_s->moveCounter / 2]) << endl;
				b->printBoard();
				cout << "###############" << endl;
			}
		}

		// SEARCH POSITION
		if (m == "s") {
			s->depth = 10;
			s->startTime = getTimeMs();
			s->stopTime = getTimeMs() + 10000;

			s->timeSet = false;
			s->depthSet = true;
			search(b, s);
			continue;
		}

		if (m == "uci") {
			uciMode(b, s);
		}

		// PRINT PV LINE FROM CURRENT BOARD
		if (m == "pv") {
			int d = getPVLine(b, MAX_DEPTH);
			cout << "PV line length " << d << endl;
			for (int i = 0; i < d; i++) {
				cout << "Best move " << i << " is: ";
				printMove(b->pvArray[i]);
				cout << endl;
			}
			continue;
		}

		if (m == "perft") {
			cout << "Perft this position to depth ";
			getline(cin, m);
			if (stoi(m) >= 1 && stoi(m) <= 15) {
				dividePerft(b, "-1", stoi(m));
			}
			continue;
		}

		if (m == "reps") {
			cout << "Board is three fold repetition " << isThreeFoldRepetition(b) << endl;
			cout << "Board is repetition " << isRepetition(b) << endl;
		}

		if (m == "0000") {
			b->pushNull();

			cout << "Pushed " << getStringMove(b->parseMove(m)) << " on board." << endl;
			b->printBoard();

			continue;
		}

		if (m == "p") {
			b->printBoard();
			continue;
		}

		// PUSH MOVE IN ALGEBRAIC NOTATION
		MOVE_S _move_s[1];
		generateMoves(b, _move_s);

		int parsedMove = b->parseMove(m);
		bool flag_found = false;
		for (int i = 0; i < _move_s->moveCounter; i++) {
			if (parsedMove == _move_s->moveList[i]) {
				flag_found = true;
				//storeTT(b, parsedMove);

				if (!b->push(parsedMove)) {
					cout << "Move " << getStringMove(parsedMove) << " remains check or is no valid move. Try again." << endl;
					continue;
				}

				if (isThreeFoldRepetition(b)) {
					cout << "Position was repeated the third time. Both players can claim draw." << endl;
				}

				cout << "Pushed " << getStringMove(parsedMove) << " on board." << endl;
				b->printBoard();
				break;
			}
		}

		if (!flag_found) cout << "Move or command not valid. Try again." << endl;
	}
}

void dividePerft(Board* b, string fen, int depth) {

	string move = "";
	Perft p;

	// parse fen if valid
	if (fen != "-1") {
		b->parseFen(fen);
	}

	while (depth) {

		if (move != "") {
			int parsedMove = b->parseMove(move);
			b->push(parsedMove);
			depth--;
		}

		p.perftRoot(b, depth);

		cout << "\nDivide at move ";
		getline(cin, move);
	}

	cout << "Reached depth 0" << endl;
	return;

}
