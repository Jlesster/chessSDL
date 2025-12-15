#include "board.h"

Board::Board() {
  char start [8][8] = {
    {'r','n','b','q','k','b','n','r'},
    {'p','p','p','p','p','p','p','p'},
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
    {'P','P','P','P','P','P','P','P'},
    {'R','N','B','Q','K','B','N','R'},
  };

  for(int row = 0; row < 8; row++) {
    for(int col = 0; col < 8; col++) {
      layout[row][col] = start[row][col];
    }
  }
}
char Board::get(int row, int col) const {
  return layout[row][col];
}
void Board::set(int row, int col, char piece) {
  layout[row][col] = piece;
}

