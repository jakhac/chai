#include "magics.h"

U64 rook_magic_gen[64];

U64 index_to_U64(int index, int bits, U64 m) {
    int i, j;
    U64 result = 0ULL;
    for (i = 0; i < bits; i++) {
        j = popBit(&m);
        if (index & (1 << i)) result |= (1ULL << j);
    }
    return result;
}

U64 rmask(int sq) {
    U64 result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 6; r++) result |= (1ULL << (fl + r * 8));
    for (r = rk - 1; r >= 1; r--) result |= (1ULL << (fl + r * 8));
    for (f = fl + 1; f <= 6; f++) result |= (1ULL << (f + rk * 8));
    for (f = fl - 1; f >= 1; f--) result |= (1ULL << (f + rk * 8));
    return result;
}

U64 ratt(int sq, U64 block) {
    U64 result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 7; r++) {
        result |= (1ULL << (fl + r * 8));
        if (block & (1ULL << (fl + r * 8))) break;
    }
    for (r = rk - 1; r >= 0; r--) {
        result |= (1ULL << (fl + r * 8));
        if (block & (1ULL << (fl + r * 8))) break;
    }
    for (f = fl + 1; f <= 7; f++) {
        result |= (1ULL << (f + rk * 8));
        if (block & (1ULL << (f + rk * 8))) break;
    }
    for (f = fl - 1; f >= 0; f--) {
        result |= (1ULL << (f + rk * 8));
        if (block & (1ULL << (f + rk * 8))) break;
    }
    return result;
}

int transform(U64 b, U64 magic, int bits) {
#if defined(USE_32_BIT_MULTIPLICATIONS)
    return
        (unsigned)((int)b * (int)magic ^ (int)(b >> 32) * (int)(magic >> 32)) >> (32 - bits);
#else
    return (int)((b * magic) >> (64 - bits));
#endif
}

U64 find_magic(int sq, int m) {
    U64 mask, b[4096], a[4096], used[4096], magic;
    int i, j, k, n, fail;

    //mask = bishop ? bmask(sq) : rmask(sq);
    mask = rmask(sq);
    n = countBits(mask);

    for (i = 0; i < (1 << n); i++) {
        b[i] = index_to_U64(i, n, mask);
        a[i] = ratt(sq, b[i]);
    }
    for (k = 0; k < 100000000; k++) {
        magic = randomFewBits();
        if (countBits((mask * magic) & 0xFF00000000000000ULL) < 6) continue;
        for (i = 0; i < 4096; i++) used[i] = 0ULL;
        for (i = 0, fail = 0; !fail && i < (1 << n); i++) {
            j = transform(b[i], magic, m);
            if (used[j] == 0ULL) used[j] = a[i];
            else if (used[j] != a[i]) fail = 1;
        }
        if (!fail) return magic;
    }
    printf("***Failed***\n");
    return 0ULL;
}

void magic() {
    int square;

    printf("const U64 RMagic[64] = {\n");
    for (square = 0; square < 64; square++) {
        rook_magic_gen[square] = find_magic(square, RBits[square]);
        printf("  0x%llxULL,\n", rook_magic_gen[square]);
    }
    printf("};\n\n");
}
