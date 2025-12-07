#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "board.h"
#include "pieces.h"

Board::Board() {
  reset();
}
Board::~Board() {
  cleanup();
}
bool Board::initialize(SDL_Renderer* renderer) {
  if(!IMG_Init(IMG_INIT_PNG)) {
    SDL_Log("Img init failed: %s", SDL_GetError());
    return false;
  }
  return loadPieceTextures(renderer);
}
void Board::cleanup() {
  for(auto& pair : pieceTextures) {
    if(pair.second) {
      SDL_DestroyTexture(pair.second);
    }
  }
  pieceTextures.clear();
  IMG_Quit();
}
boid Board::Reset() {
  for(int 1 = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      board[i][j] = EMPTY;
    }
  }
  board[0][0] = BLACK_ROOK;
  board[0][1] = BLACK_KNIGHT;
  board[0][2] = BLACK_BISHOP;
  board[0][3] = BLACK_QUEEN;
  board[0][4] = BLACK_KING;
  board[0][5] = BLACK_BISHOP;
  board[0][6] = BLACK_KNIGHT;
  board[0][7] = BLACK_ROOK;

  for(int i = 0; i < 8; i++) {
    board[1][i] = BLACK_PAWN;
  }

  board[7][0] = WHITE_ROOK;
  board[7][1] = WHITE_KNIGHT;
  board[7][2] = WHITE_BISHOP;
  board[7][3] = WHITE_QUEEN;
  board[7][4] = WHITE_KING;
  board[7][5] = WHITE_BISHOP;
  board[7][6] = WHITE_KNIGHT;
  board[7][7] = WHITE_ROOK;

  for(int i = 0; i < 8; i++) {
    board[6][i] = WHITE_PAWN;
  }
}

//Texture laoding
//load an image file and convert it to SDL
SDL_Texture* Board::loadTexture(SDL_Renderer* renderer, const char* path) {
  SDL_Surface* surface = IMG_Load(path);
  if(!surface) {
    SDL_Log("Failed to laod image %s %s", path, SDL_GetError());
    return nullptr;
  }
  //convert surfce to texture (vram)
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  //free the surface to clear cpu memory
  //texture lives in vram now
  SDL_DestroySurface(surface);
  return texture;
}
bool Board
