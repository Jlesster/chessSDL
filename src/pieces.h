#ifndef PIECES_H
#define PIECES_H

enum pieceType {
  EMPTY =0,
  WHITE_PAWN, WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING,
  BLACK_PAWN, BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING
};
enum pieceColor {
  COLOR_NONE,
  COLOR_WHITE,
  COLOR_BLACK
};

//returns name of the piece (PawnWHT)
const char* getPieceName(pieceType piece);

//gets the color of a piece (WHT, BLK)
pieceColor getPieceColor(pieceType piece);

//checking pieces
bool isPawn(pieceType piece);
bool isRook(pieceType piece);
bool isKnight(pieceType piece);
bool isBishop(pieceType piece);
bool isQueen(pieceType piece);
bool isKing(pieceType piece);

bool IsValidPawnMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidRookMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidKnightMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidBishopMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidQueenMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidKingMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);

#endif // !PIECES_H
