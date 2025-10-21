#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "bitboard.h"

/* ========= Phase 1: Bit Manipulation ========= */
unsigned int SetBit(unsigned int v, int p){ if(p<0||p>=32) return v; return v | (1U<<p); }
unsigned int ClearBit(unsigned int v, int p){ if(p<0||p>=32) return v; return v & ~(1U<<p); }
unsigned int ToggleBit(unsigned int v, int p){ if(p<0||p>=32) return v; return v ^ (1U<<p); }
int GetBit(unsigned int v, int p){ if(p<0||p>=32) return -1; return (v>>p)&1U; }
int CountBits(unsigned int v){ int c=0; while(v){ c += (v&1U); v >>= 1; } return c; }
unsigned int ShiftLeft(unsigned int v, int k){ if(k<0||k>=32) return v; return v<<k; }
unsigned int ShiftRight(unsigned int v, int k){ if(k<0||k>=32) return v; return v>>k; }
void PrintBinary(unsigned int v){ for(int i=31;i>=0;--i){ printf("%d",(v>>i)&1U); if(i%8==0) printf(" "); } printf("\n"); }
void PrintHex(unsigned int v){ printf("0x%X\n", v); }

/* ========= Helpers for bitboard ========= */
static inline int on_board(int r,int c){ return r>=0&&r<8&&c>=0&&c<8; }
static inline int is_dark(int r,int c){ return ((r+c)&1)==1; }
static inline int rc_to_pos(int r,int c){ return r*8 + c; }
static inline uint64_t bit_at(int pos){ return 1ULL << pos; }
static inline int has_any(uint64_t bb, int pos){ return ((bb >> pos) & 1ULL) != 0ULL; }

static inline int is_occupied(const GameState* g, int pos){
    uint64_t all = g->p1 | g->p1K | g->p2 | g->p2K;
    return ((all >> pos) & 1ULL) != 0ULL;
}
static inline int is_p1_piece(const GameState* g, int pos){ return has_any(g->p1,pos) || has_any(g->p1K,pos); }
static inline int is_p2_piece(const GameState* g, int pos){ return has_any(g->p2,pos) || has_any(g->p2K,pos); }
static inline int is_king(const GameState* g, int pos, int player){
    return player==1 ? has_any(g->p1K,pos) : has_any(g->p2K,pos);
}

void print_board(const GameState* g){
    printf("  0 1 2 3 4 5 6 7\n");
    for(int r=0;r<8;++r){
        printf("%d ", r);
        for(int c=0;c<8;++c){
            if(!is_dark(r,c)){ printf(". "); continue; }
            int pos = rc_to_pos(r,c);
            if( ((g->p1   >> pos)&1ULL) ) { printf("r "); }
            else if( ((g->p1K  >> pos)&1ULL) ) { printf("R "); }
            else if( ((g->p2   >> pos)&1ULL) ) { printf("b "); }
            else if( ((g->p2K  >> pos)&1ULL) ) { printf("B "); }
            else { printf("- "); }
        }
        printf("\n");
    }
}

void init_game(GameState* g){
    g->p1 = 0; g->p1K = 0; g->p2 = 0; g->p2K = 0; g->turn = 1;
    for(int r=0;r<3;++r) for(int c=0;c<8;++c) if(is_dark(r,c)) g->p1 |= bit_at(rc_to_pos(r,c));
    for(int r=5;r<8;++r) for(int c=0;c<8;++c) if(is_dark(r,c)) g->p2 |= bit_at(rc_to_pos(r,c));
}

void promote_if_needed(GameState* g){
    uint64_t row7 = 0xFF00000000000000ULL; // r=7
    uint64_t row0 = 0x00000000000000FFULL; // r=0
    uint64_t toK1 = g->p1 & row7; if(toK1){ g->p1 &= ~toK1; g->p1K |= toK1; }
    uint64_t toK2 = g->p2 & row0; if(toK2){ g->p2 &= ~toK2; g->p2K |= toK2; }
}

int winner(const GameState* g){
    if( (g->p1|g->p1K)==0 ) return 2;
    if( (g->p2|g->p2K)==0 ) return 1;
    return 0;
}

/* diagonal step or single jump; row/col input */
int move_piece(GameState* g, int fr,int fc,int tr,int tc){
    if(!on_board(fr,fc) || !on_board(tr,tc)) return 0;
    if(!is_dark(fr,fc) || !is_dark(tr,tc))   return 0;

    int from = rc_to_pos(fr,fc);
    int to   = rc_to_pos(tr,tc);
    if(is_occupied(g,to)) return 0;

    int dr = tr - fr, dc = tc - fc;
    if(abs(dc)!=1 && abs(dc)!=2) return 0;    // diagonal only
    if(abs(dc)==1 && abs(dr)!=1) return 0;    // step: 1 row
    if(abs(dc)==2 && abs(dr)!=2) return 0;    // jump: 2 rows

    if(g->turn==1){
        if(!is_p1_piece(g,from)) return 0;
        int king = is_king(g,from,1);
        if(!king && dr!=1 && !(abs(dc)==2 && dr==2)) return 0; // red moves down unless king
        if(abs(dc)==2){ // capture
            int mr=(fr+tr)/2, mc=(fc+tc)/2, mid=rc_to_pos(mr,mc);
            if(!is_p2_piece(g,mid)) return 0;
            if(has_any(g->p2K, mid)) g->p2K &= ~bit_at(mid); else g->p2 &= ~bit_at(mid);
        }
        if(has_any(g->p1K, from)){ g->p1K &= ~bit_at(from); g->p1K |= bit_at(to); }
        else                     { g->p1  &= ~bit_at(from); g->p1  |= bit_at(to); }
        promote_if_needed(g);
        g->turn = 2;
        return 1;
    }else{
        if(!is_p2_piece(g,from)) return 0;
        int king = is_king(g,from,2);
        if(!king && dr!=-1 && !(abs(dc)==2 && dr==-2)) return 0; // black moves up unless king
        if(abs(dc)==2){
            int mr=(fr+tr)/2, mc=(fc+tc)/2, mid=rc_to_pos(mr,mc);
            if(!is_p1_piece(g,mid)) return 0;
            if(has_any(g->p1K, mid)) g->p1K &= ~bit_at(mid); else g->p1 &= ~bit_at(mid);
        }
        if(has_any(g->p2K, from)){ g->p2K &= ~bit_at(from); g->p2K |= bit_at(to); }
        else                     { g->p2  &= ~bit_at(from); g->p2  |= bit_at(to); }
        promote_if_needed(g);
        g->turn = 1;
        return 1;
    }
}

int main(void){
    GameState g; init_game(&g);

    printf("Welcome to BitBoard Checkers!\n");
    print_board(&g);

    int fr,fc,tr,tc;
    while(1){
        printf("Player %d move [from_row from_col to_row to_col]: ", g.turn==1?1:2);
        if(scanf("%d %d %d %d",&fr,&fc,&tr,&tc)!=4) break;

        if(move_piece(&g,fr,fc,tr,tc)){
            print_board(&g);
        }else{
            printf("Invalid move!\n");
        }

        int w = winner(&g);
        if(w){ printf("Player %d wins!\n", w); break; }
    }
    printf("Game over.\n");
    return 0;
}
