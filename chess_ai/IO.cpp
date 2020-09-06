#include "IO.h"

/*
Print square in algebraic notation
*/
char* IO::printSquare(const int square)
{
	static char squareStr[3];

	// get file and rank
	int file = fileBoard[square];
	int rank = rankBoard[square];

	sprintf_s(squareStr, "%c%c", ('a' + file), ('1' + rank));

	return squareStr;
}

char* IO::printMove(const int move) {

	static char moveStr[6];

	int ff = fileBoard[FROMSQ(move)];
	int rf = rankBoard[FROMSQ(move)];
	int ft = fileBoard[TOSQ(move)];
	int rt = rankBoard[TOSQ(move)];

	int promoted = PROMOTED(move);

	if (promoted) {
		char pchar = 'q';
		if (bd.isK(promoted)) {
			pchar = 'n';
		}
		else if (bd.isRQ(promoted) && !bd.isBQ(promoted)) {
			pchar = 'r';
		}
		else if (!bd.isRQ(promoted) && bd.isBQ(promoted)) {
			pchar = 'b';
		}
		sprintf_s(moveStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
	}
	else {
		sprintf_s(moveStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
	}

	return moveStr;
}
