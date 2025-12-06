#include "pieces.h"
#include <cmath>

//creating map for piece name to filename like a hashmap of sorts
const char* getPieceName(pieceType piece) {
  switch(piece) {
    case BLACK_BISHOP: return "BishopBLK";
    case BLACK_KING: return "KingBLK";
    case BLACK_KNIGHT: return "KnightBLK";
    case BLACK_PAWN: return "PawnBLK";
    case BLACK_QUEEN: return "QueenBLK";
    case BLACK_ROOK: return "RookBLK";
    case WHITE_BISHOP: return "BishopWHT";
    case WHITE_KING: return "KingWHT";
    case WHITE_KNIGHT: return "KnightWHT";
    case WHITE_PAWN: return "PawnWHT";
    case WHITE_QUEEN: return "QueenWHT";
    case WHITE_ROOK: return "RookWHT";
    default: return nullptr;
  }
}
pieceColor getPieceColor(pieceType piece) {
  if(piece >= WHITE_PAWN && piece <= WHITE_KING) return COLOR_WHITE;
  if(piece >= BLACK_PAWN && piece <= BLACK_KING) return COLOR_BLACK;
  return COLOR_NONE;
}
bool isPawn(pieceType piece) {
  return piece == WHITE_PAWN || piece == BLACK_PAWN;
}
bool isRook(pieceType piece) {
  return piece == WHITE_ROOK || piece == BLACK_ROOK;
}
bool isKnight(pieceType piece) {
  return piece == WHITE_KNIGHT || piece == BLACK_KNIGHT;
}
bool isQueen(pieceType piece) {
  return piece == WHITE_QUEEN || piece == BLACK_QUEEN;
}
bool isKing(pieceType piece) {
  return piece ==WHITE_KING || piece == BLACK_KING;
}
bool isBishop(pieceType piece) {
  return piece == WHITE_BISHOP || piece == BLACK_BISHOP;
}
