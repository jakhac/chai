#include "move.h"

/*
Return a bitmove with all attributes as parameters. NOT USED, REPLACED BY MACRO
*/
int Move::createBitMove(int from, int to, int cap, int prom, int flag) {
	return (from | (to << 7) | (cap << 14) | (prom << 20) | flag);
}
