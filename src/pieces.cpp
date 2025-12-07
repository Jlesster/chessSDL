#include "pieces.h"
#include <cmath>
#include <cstdlib>

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

bool IsValidPawnMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
  pieceType piece = board[fromRow][fromCol];
  pieceType target = board[toRow][toCol];

  //white pawns up 1 black pawns down 1
  int direction = (piece == WHITE_PAWN) ? -1 : 1;

  //starting row 2 square move
  int startRow = (piece == WHITE_PAWN) ? 6 : 1;

  //move one square if empty
  if(toCol == fromCol && toRow == fromRow + direction && target == EMPTY) {
     return true;
  }
  //move forward 2 squares from starting position if bot squares are empty
  if(fromRow == startRow && toCol == fromCol && toRow == fromRow + 2 * direction && target == EMPTY && board[fromRow + direction][fromCol] == EMPTY) {
    return true;
  }
  //capture diagonally if enemy piece
  if(abs(toCol - fromCol) == 1 && toRow == fromRow + direction && target == EMPTY) {
    return true;
  }
  return false;
}

bool IsValidRookMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
  if(fromRow != toRow && fromCol != toCol) return false;

  int rowDir = (toRow > fromRow) ? 1 : (toRow < fromRow) ? -1 : 0;
  int colDir = (toCol > fromCol) ? 1 : (toCol < fromCol) ? -1 : 0;

  int currentRow = fromRow + rowDir;
  int currentCol = fromCol + colDir;

  while(currentRow != toRow || currentCol != toCol) {
    if(board[currentRow][currentCol] != EMPTY) return false;

    currentRow += rowDir;
    currentCol += colDir;
  }
  return true;
}
bool IsValidKnightMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
  int rowDiff = abs(toRow - fromRow);
  int colDiff = abs(toCol - fromCol);

  return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}
bool isValidBishopMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
  if(abs(toRow - fromRow) != abs(toCol - fromCol)) return false;

  int rowDir = (toRow > fromRow) ? 1 : -1;
  int colDir = (toCol > fromCol) ? 1 : -1;

  int currentRow = fromRow + rowDir;
  int currentCol = fromCol + colDir;

  while(currentRow != toRow || currentCol != toCol) {
    if(board[currentRow][currentCol] != EMPTY) return false;
    currentRow += rowDir;
    currentCol += colDir;
  }
  return true;
}
bool IsValidQueenMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
  return isValidBishopMove(board, fromRow, fromCol, toRow, toCol) ||
         IsValidRookMove(board, fromRow, fromCol, toRow, toCol);
}
bool isValidKingMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
  int rowDiff = abs(toRow - fromRow);
  int colDiff = abs(toCol - fromCol);

  return rowDiff <= 1 && colDiff <= 1;
}
bool isValidMove(pieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
  pieceType piece = board[fromRow][fromCol];
  pieceType target = board[toRow][toCol];

  if(fromRow == toRow && fromCol == toCol) return false;

  if(target != EMPTY) {
    if(getPieceColor(piece) == getPieceColor(target)) return false;
  }
  if(isPawn(piece)) return IsValidPawnMove(board, fromRow, fromCol, toRow, toCol);
  if(isBishop(piece)) return IsValidBishopMove(board, fromRow, fromCol, toRow, toCol);
  if(isKnight(piece)) return IsValidKnightMove(board, fromRow, fromCol, toRow, toCol);
  if(isQueen(piece)) return IsValidQueenMove(board, fromRow, fromCol, toRow, toCol);
  if(isRook(piece)) return IsValidRookMove(board, fromRow, fromCol, toRow, toCol);
  if(isKing(piece)) return IsValidKingMove(board, fromRow, fromCol, toRow, toCol);
  return false;
}
