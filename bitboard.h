#ifndef BITBOARD_H
#define BITBOARD_H
#include <stdint.h>

typedef struct {
    uint64_t p1, p1K, p2, p2K; // red men, red kings, black men, black kings
    int turn;                  // 1 = red, 2 = black
} GameState;

/* Phase 1: bit ops */
unsigned int SetBit(unsigned int v, int p);
unsigned int ClearBit(unsigned int v, int p);
unsigned int ToggleBit(unsigned int v, int p);
int          GetBit(unsigned int v, int p);
int          CountBits(unsigned int v);
unsigned int ShiftLeft(unsigned int v, int k);
unsigned int ShiftRight(unsigned int v, int k);
void         PrintBinary(unsigned int v);
void         PrintHex(unsigned int v);

/* Phase 2: game */
void init_game(GameState* g);
void print_board(const GameState* g);
int  move_piece(GameState* g, int fr,int fc,int tr,int tc); // row/col input
void promote_if_needed(GameState* g);
int  winner(const GameState* g);

#endif
