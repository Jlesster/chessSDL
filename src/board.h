#ifndef BOARD_H
#define BOARD_H

#include <SDL3/SDL.h>
#include <map>
#include <string>
#include "pieces.h"

class Board {
public:
  Board();
  ~Board();

  bool initialize(SDL_Renderer* renderer);

  void cleanup();

  void reset();

  void draw(SDL_Renderer* renderer, int selectedRow, int selectedCol, bool pieceSelected);

  void drawPieces(SDL_Renderer* renderer);

  pieceType getPiece(int row, int col) const;

  void setPiece(int row, int col, pieceType piece);

  void movePiece(int fromRow, int fromCol, int toRow, int toCol);

  static const int squareSize = 100;
  static const int boardSize = 8;

private:
  pieceType board[8][8];

  std::map<std::string, SDL_Texture*> pieceTextures;

  bool loadPieceTextures(SDL_Renderer* renderer);

  SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path);
};
#endif // !BOARD_H
