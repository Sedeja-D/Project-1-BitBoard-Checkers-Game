# BitBoard Checkers

A checkers game built in **C** using **bitboards**.

## How to Play

Enter moves: `from_row from_col to_row to_col` (e.g., `2 1 3 0`)  
Red (`r`) moves first; Black (`b`) next.  
Diagonal moves; kings (`R`, `B`) move both directions.  
Game ends when a player has no pieces.

## Files

- `main.c` — game logic + bit operations  
- `bitboard.h` — header  
- `Makefile` — build
- `Screenshot` - game output
