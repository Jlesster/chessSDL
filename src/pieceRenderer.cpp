#include "pieceRenderer.h"
#include <SDL3/SDL_gpu.h>
#include <SDL3_image/SDL_image.h>

PieceRenderer::PieceRenderer(SDL_GPUDevice* device)
  :device(device), maxVertices(8 * 8 * 6) {

    for(int i = 0; i < 128; i++) {
      textures[i] = nullptr;
    }
    auto load = [&](char c, const char* path) {
      SDL_Surface* surf = IMG_Load(path);
      if(!surf) {
        return;
      }

      SDL_GPUTextureCreateInfo info{};
      info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
      info.width  = surf->w;
      info.height = surf->h;
      info.usage  = SDL_GPU_TEXTUREUSAGE_SAMPLER;

      textures[(unsigned char)c] = SDL_CreateGPUTexture(device, &info);

      SDL_UpdateGPUTexture(
        device,
        textures[(unsigned char)c],
        nullptr,
        surf->pixels,
        surf->pitch
      );
      SDL_DestroySurface(surf);
    };
    load('P', "assets/pieces/PawnWHT.png");
    load('B', "assets/pieces/BishopWHT.png");
    load('Q', "assets/pieces/QueenWHT.png");
    load('N', "assets/pieces/KnightWHT.png");
    load('K', "assets/pieces/KingWHT.png");
    load('R', "assets/pieces/RookWHT.png");

    load('p', "assets/pieces/PawnBLK.png");
    load('b', "assets/pieces/BishopBLK.png");
    load('q', "assets/pieces/QueenBLK.png");
    load('n', "assets/pieces/KnightBLK.png");
    load('k', "assets/pieces/KingBLK.png");
    load('r', "assets/pieces/RookBLK.png");
  }
