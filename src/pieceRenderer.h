#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include "board.h"

class PieceRenderer {
  public:
    PieceRenderer(SDL_GPUDevice* device);
    ~PieceRenderer();

    void draw(SDL_GPURenderPass* rPass, const Board& board);

  private:
    SDL_GPUDevice* device;
    SDL_GPUTexture* textures[128];
    SDL_GPUBuffer* vBuffer;
    SDL_GPUTransferBuffer* tBuffer;

    Uint32 maxVertices;
};
