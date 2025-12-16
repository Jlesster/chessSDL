#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <vector>
#include "board.h"

class PieceRenderer {
  public:
    PieceRenderer(SDL_GPUDevice* device, SDL_Window* window);
    ~PieceRenderer();

    void updateVertices(SDL_GPUCommandBuffer* cmd, const Board& board);
    void draw(SDL_GPURenderPass* rPass);

  private:
    SDL_GPUDevice* device;
    SDL_GPUTexture* textures[128];
    SDL_GPUSampler* sampler;
    SDL_GPUGraphicsPipeline* pipeline;
    SDL_GPUBuffer* vBuffer;
    SDL_GPUTransferBuffer* tBuffer;

    Uint32 maxVertices;
    Uint32 currentVertexCount;

    std::vector<char> pieceOrder;
};
