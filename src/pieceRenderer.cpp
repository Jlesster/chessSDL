#include "pieceRenderer.h"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <vector>

struct PieceVertex {
  float x, y, z;
  float u, v;
};

PieceRenderer::~PieceRenderer() {
  if(vBuffer) SDL_ReleaseGPUBuffer(device, vBuffer);
  if(tBuffer) SDL_ReleaseGPUTransferBuffer(device, tBuffer);
  if(pipeline) SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
  if(sampler) SDL_ReleaseGPUSampler(device, sampler);


  for(int i = 0; i < 128; i++) {
      if(textures[i]) {
          SDL_ReleaseGPUTexture(device, textures[i]);
      }
  }
}

PieceRenderer::PieceRenderer(SDL_GPUDevice* device, SDL_Window* window)
  :device(device), maxVertices(8 * 8 * 6), currentVertexCount(0) {

    SDL_GPUBufferCreateInfo vbInfo{};
    vbInfo.size = maxVertices * sizeof(PieceVertex);
    vbInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vBuffer = SDL_CreateGPUBuffer(device, &vbInfo);

    SDL_GPUTransferBufferCreateInfo tbInfo{};
    tbInfo.size = maxVertices * sizeof(PieceVertex);
    tbInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tBuffer = SDL_CreateGPUTransferBuffer(device, &tbInfo);

    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler = SDL_CreateGPUSampler(device, &samplerInfo);

    for(int i = 0; i < 128; i++) {
      textures[i] = nullptr;
    }
    auto load = [&](char c, const char* path) {
      SDL_Surface* surf = IMG_Load(path);
      if(!surf) {
        SDL_Log("Failed to load %s: %s", path, SDL_GetError());
        return;
      }
      SDL_Log("Loaded %s: %dx%d, format: %s", path, surf->w, surf->h, SDL_GetPixelFormatName(surf->format));

      SDL_GPUTextureCreateInfo info{};
      info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
      info.width  = surf->w;
      info.height = surf->h;
      info.layer_count_or_depth = 1;
      info.num_levels = 1;
      info.usage  = SDL_GPU_TEXTUREUSAGE_SAMPLER;

      SDL_GPUTexture* tex = SDL_CreateGPUTexture(device, &info);
      if(!tex) {
        SDL_Log("ERROR: Failed to create GPU texture for %c", c);
      } else {
        SDL_Log("Created GPU texture for piece '%c': %p", c, (void*)tex);
      }
      textures[(unsigned char)c] = tex;

      SDL_GPUTransferBufferCreateInfo tbInfo{};
      tbInfo.size = surf->pitch * surf->h;
      tbInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

      SDL_GPUTransferBuffer* tBuffer = SDL_CreateGPUTransferBuffer(device, &tbInfo);

      void* dst = SDL_MapGPUTransferBuffer(device, tBuffer, false);
      SDL_memcpy(dst, surf->pixels, surf->pitch * surf->h);
      SDL_UnmapGPUTransferBuffer(device, tBuffer);

      SDL_GPUTextureTransferInfo src{};
      src.transfer_buffer = tBuffer;
      src.offset = 0;
      src.pixels_per_row = surf->w;
      src.rows_per_layer = surf->h;

      SDL_GPUTextureRegion dstRegion{};
      dstRegion.texture = tex;
      dstRegion.w = surf->w;
      dstRegion.h = surf->h;
      dstRegion.d = 1;
      dstRegion.x = 0;
      dstRegion.y = 0;
      dstRegion.z = 0;

      SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
      SDL_GPUCopyPass* cPass = SDL_BeginGPUCopyPass(cmd);
      SDL_UploadToGPUTexture(cPass, &src, &dstRegion, true);

      SDL_EndGPUCopyPass(cPass);
      SDL_SubmitGPUCommandBuffer(cmd);
      SDL_WaitForGPUIdle(device);

      SDL_ReleaseGPUTransferBuffer(device, tBuffer);
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

    size_t vertexCodeSize;
    void* vertexCode = SDL_LoadFile("shaders/pieceVertex.spv", &vertexCodeSize);
    if(!vertexCode) {
      SDL_Log("Failed to load piece vertex shader");
      return;
    }

    SDL_GPUShaderCreateInfo vertexInfo{};
    vertexInfo.code = (Uint8*)vertexCode;
    vertexInfo.code_size = vertexCodeSize;
    vertexInfo.entrypoint = "main";
    vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    vertexInfo.num_samplers = 0;
    vertexInfo.num_storage_buffers = 0;
    vertexInfo.num_storage_textures = 0;
    vertexInfo.num_uniform_buffers = 0;
    SDL_GPUShader* vertexShader = SDL_CreateGPUShader(device, &vertexInfo);
    SDL_free(vertexCode);

    size_t fragmentCodeSize;
    void* fragmentCode = SDL_LoadFile("shaders/pieceFragment.spv", &fragmentCodeSize);
    if(!fragmentCode) {
      SDL_Log("Failed to load piece fragment shader");
      return;
    }

    SDL_GPUShaderCreateInfo fragInfo{};
    fragInfo.code = (Uint8*)fragmentCode;
    fragInfo.code_size = fragmentCodeSize;
    fragInfo.entrypoint = "main";
    fragInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    fragInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    fragInfo.num_samplers = 1;
    fragInfo.num_storage_buffers = 0;
    fragInfo.num_storage_textures = 0;
    fragInfo.num_uniform_buffers = 0;
    SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragInfo);
    SDL_free(fragmentCode);

    SDL_GPUGraphicsPipelineCreateInfo pInfo{};
    SDL_zero(pInfo);
    pInfo.vertex_shader = vertexShader;
    pInfo.fragment_shader = fragmentShader;
    pInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    pInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pInfo.multisample_state.sample_count= SDL_GPU_SAMPLECOUNT_1;

    SDL_GPUVertexBufferDescription vBufferDesc{};
    vBufferDesc.slot = 0;
    vBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vBufferDesc.instance_step_rate = 0;
    vBufferDesc.pitch = sizeof(PieceVertex);

    SDL_GPUVertexAttribute vAttribs[2];
    vAttribs[0].buffer_slot = 0;
    vAttribs[0].location = 0;
    vAttribs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vAttribs[0].offset = 0;

    vAttribs[1].buffer_slot = 0;
    vAttribs[1].location = 1;
    vAttribs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vAttribs[1].offset = sizeof(float) * 3;

    pInfo.vertex_input_state.num_vertex_buffers = 1;
    pInfo.vertex_input_state.vertex_buffer_descriptions = &vBufferDesc;
    pInfo.vertex_input_state.num_vertex_attributes = 2;
    pInfo.vertex_input_state.vertex_attributes = vAttribs;

    SDL_GPUColorTargetDescription cTargetDesc{};
    cTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(device, window);
    cTargetDesc.blend_state.enable_blend = true;
    cTargetDesc.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    cTargetDesc.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    cTargetDesc.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    cTargetDesc.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    cTargetDesc.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    cTargetDesc.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    pInfo.target_info.num_color_targets = 1;
    pInfo.target_info.color_target_descriptions = &cTargetDesc;

    pipeline = SDL_CreateGPUGraphicsPipeline(device, &pInfo);
    if(!pipeline) {
      SDL_Log("Piece pipeline create failed: %s", SDL_GetError());
      return;
    }


    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);
  }
void PieceRenderer::updateVertices(SDL_GPUCommandBuffer* cmd, const Board& board) {
  std::vector<PieceVertex> vertices;
  vertices.reserve(maxVertices);
  pieceOrder.clear();

  float square  = 2.0f / 8.0f;

  for(int row = 0; row < 8; row++) {
    for(int col = 0; col < 8; col++) {
      char piece = board.get(row, col);
      if(!piece) continue;

      pieceOrder.push_back(piece);

      float x0 = -1.0f + col * square;
      float y0 = 1.0f - row * square;
      float x1 = x0 + square;
      float y1 = y0 - square;

      vertices.push_back({x0,y0,0, 0,0});
      vertices.push_back({x1,y0,0, 1,0});
      vertices.push_back({x0,y1,0, 0,1});

      vertices.push_back({x1,y0,0, 1,0});
      vertices.push_back({x1,y1,0, 1,1});
      vertices.push_back({x0,y1,0, 0,1});
    }
  }

  currentVertexCount = static_cast<Uint32>(vertices.size());
  // SDL_Log("Updating %d vertices for %zu pieces", currentVertexCount, pieceOrder.size());
  if(vertices.empty()) {
    return;
  }

  PieceVertex* dst = (PieceVertex*)SDL_MapGPUTransferBuffer(device, tBuffer, false);
  SDL_memcpy(dst,vertices.data(),vertices.size() * sizeof(PieceVertex));
  SDL_UnmapGPUTransferBuffer(device, tBuffer);

  SDL_GPUTransferBufferLocation src{};
  src.transfer_buffer = tBuffer;
  src.offset = 0;

  SDL_GPUBufferRegion dstRegion{};
  dstRegion.buffer = vBuffer;
  dstRegion.offset = 0;
  dstRegion.size = vertices.size() * sizeof(PieceVertex);

  SDL_GPUCopyPass* cPass = SDL_BeginGPUCopyPass(cmd);
  SDL_UploadToGPUBuffer(cPass, &src, &dstRegion, false);
  SDL_EndGPUCopyPass(cPass);
}

void PieceRenderer::draw(SDL_GPURenderPass* rPass) {
  if(currentVertexCount == 0) {
    SDL_Log("No vertices to draw");
    return;
  }
  SDL_Log("Drawing pieces");

  SDL_BindGPUGraphicsPipeline(rPass, pipeline);

  SDL_GPUBufferBinding binding{};
  binding.buffer = vBuffer;
  binding.offset = 0;
  SDL_BindGPUVertexBuffers(rPass, 0, &binding, 1);

  for(size_t i =0; i < pieceOrder.size(); i++) {
    char piece = pieceOrder[i];
    SDL_GPUTexture* tex = textures[(unsigned char)piece];
    SDL_Log("Drawing piece %c, textire: %p", piece, (void*)tex);

    if(tex) {
      SDL_GPUTextureSamplerBinding texBinding{};
      texBinding.texture = tex;
      texBinding.sampler = sampler;
      SDL_BindGPUFragmentSamplers(rPass, 0, &texBinding, 1);

      SDL_DrawGPUPrimitives(rPass, 6, 1, i * 6, 0);
    } else {
      SDL_Log("WARNING: no texture for piece '%c'", piece);
    }
  }
}

