#define SDL_MAIN_USE_CALLBACKS
#include <vector>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window* window;
SDL_GPUDevice* device;
SDL_GPUBuffer* vBuffer;
SDL_GPUTransferBuffer* transferBuffer;
SDL_GPUGraphicsPipeline* gPipeline;

Uint32 vertexCount = 0;

struct vertex {
  float x, y, z;
  float r, g, b, a;
};


// hard coding static vertices for a shape
// static vertex vertices[] {
//   {-1.00f,  1.00f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f}, //top left
//   {-0.75f,  1.00f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f}, //top right
//   {-1.00f,  0.75f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f}, //bottom left
//   {-0.75f,  0.75f,  0.0f,  0.0f,  0.0f,  1.0f, 1.0f}, //bottom right
// };

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  (void)appstate;
  (void)argc;
  (void)argv;

  window = SDL_CreateWindow("Chesster", 800, 800, 0);
  if(!window) {
    SDL_Log("Err init window: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL);
  if(!device) {
    SDL_Log("Err init GPU device: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if(!SDL_ClaimWindowForGPUDevice(device, window)) {
    SDL_Log("Err claiming window for device: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  //dynamic vertices calculations
  std::vector<vertex> vertices;
  float squareSize = 2.0f / 8.0f;
  float startX = -1.0f;
  float startY =  1.0f;
  for(int row = 0; row < 8; row++) {
    for(int col = 0; col < 8; col++) {
      float x0 = startX + col * squareSize;
      float y0 = startY - row * squareSize;
      float x1 = x0 + squareSize;
      float y1 = y0 - squareSize;

      bool isLight = ((row + col) % 2 == 0);

      float r = isLight ? 0.9f : 0.2f;
      float g = isLight ? 0.9f : 0.2f;
      float b = isLight ? 0.9f : 0.2f;

      vertices.push_back({x0, y0, 0.0f, r, g, b, 1.0f}); //top left
      vertices.push_back({x1, y0, 0.0f, r, g, b, 1.0f}); //top right
      vertices.push_back({x0, y1, 0.0f, r, g, b, 1.0f}); //bottom left

      vertices.push_back({x1, y0, 0.0f, r, g, b, 1.0f}); //top right
      vertices.push_back({x1, y1, 0.0f, r, g, b, 1.0f}); //bottom right
      vertices.push_back({x0, y1, 0.0f, r, g, b, 1.0f}); //bottom left
    }
  }

  vertexCount = static_cast<Uint32>(vertices.size());

  //bufferInfo below
  SDL_GPUBufferCreateInfo bufferInfo{};
  bufferInfo.size = vertices.size() * sizeof(vertex);
  bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  vBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

  SDL_GPUCommandBuffer* initCmd = SDL_AcquireGPUCommandBuffer(device);
  SDL_GPUCopyPass* cPass = SDL_BeginGPUCopyPass(initCmd);

  //transferInfo below
  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.size = vertices.size() * sizeof(vertex);
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

  //vertexData below
  vertex* data = (vertex*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);
  SDL_memcpy(data, vertices.data(), (vertices.size() * sizeof(vertex)));
  SDL_UnmapGPUTransferBuffer(device, transferBuffer);

  //transferBuffer below
  SDL_GPUTransferBufferLocation location{};
  location.transfer_buffer = transferBuffer;
  location.offset = 0;

  //bufferRegion below
  SDL_GPUBufferRegion region{};
  region.buffer = vBuffer;
  region.size = vertices.size() * sizeof(vertex);
  region.offset = 0;

  SDL_UploadToGPUBuffer(cPass, &location, &region, true);
  SDL_EndGPUCopyPass(cPass);
  SDL_SubmitGPUCommandBuffer(initCmd);

  size_t vertexCodeSize;
  void* vertexCode = SDL_LoadFile("shaders/vertex.spv", &vertexCodeSize);
  if(!vertexCode) {
    SDL_Log("Failed to init vertex shaders: %s", SDL_GetError());
    return SDL_APP_FAILURE;
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

  if(!vertexShader) {
    SDL_Log("Err init vertex shader: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  size_t fragmentCodeSize;
  void* fragmentCode = SDL_LoadFile("shaders/fragment.spv", &fragmentCodeSize);
  if(!fragmentCode) {
    SDL_Log("Err init fragment shaders: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GPUShaderCreateInfo fragmentInfo{};
  fragmentInfo.code = (Uint8*)fragmentCode;
  fragmentInfo.code_size = fragmentCodeSize;
  fragmentInfo.entrypoint = "main";
  fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
  fragmentInfo.num_samplers = 0;
  fragmentInfo.num_storage_buffers = 0;
  fragmentInfo.num_storage_textures = 0;
  fragmentInfo.num_uniform_buffers = 0;
  SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragmentInfo);
  SDL_free(fragmentCode);

  if(!fragmentCode) {
    SDL_Log("failed to init fragment shader: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GPUGraphicsPipelineCreateInfo pInfo{};
  pInfo.vertex_shader = vertexShader;
  pInfo.fragment_shader = fragmentShader;

  pInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  // pInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;

  SDL_GPUVertexBufferDescription vBufferDescriptions[1];
  vBufferDescriptions[0].slot = 0;
  vBufferDescriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  vBufferDescriptions[0].instance_step_rate = 0;
  vBufferDescriptions[0].pitch = sizeof(vertex);

  SDL_GPUVertexAttribute vAttribs[2];
  vAttribs[0].buffer_slot = 0;
  vAttribs[0].location = 0;
  vAttribs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
  vAttribs[0].offset = 0;

  vAttribs[1].buffer_slot = 0;
  vAttribs[1].location = 1;
  vAttribs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
  vAttribs[1].offset = sizeof(float) * 3;

  pInfo.vertex_input_state.num_vertex_buffers = 1;
  pInfo.vertex_input_state.vertex_buffer_descriptions = vBufferDescriptions;
  pInfo.vertex_input_state.num_vertex_attributes = 2;
  pInfo.vertex_input_state.vertex_attributes = vAttribs;

  SDL_GPUColorTargetDescription cTargetDescriptions[1];
  cTargetDescriptions[0] = {};
  cTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
  cTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
  cTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  cTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  cTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  cTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  cTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);

  pInfo.target_info.num_color_targets = 1;
  pInfo.target_info.color_target_descriptions = cTargetDescriptions;

  gPipeline = SDL_CreateGPUGraphicsPipeline(device, &pInfo);

  SDL_ReleaseGPUShader(device, vertexShader);
  SDL_ReleaseGPUShader(device, fragmentShader);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  (void)appstate;

  SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
  SDL_GPUTexture* sTexture;
  Uint32 width, height;

  if(!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &sTexture, &width, &height)) {
    SDL_SubmitGPUCommandBuffer(cmd);
    return SDL_APP_CONTINUE;
  }

  SDL_GPUColorTargetInfo cTargetInfo{};
  cTargetInfo.clear_color = {248/255.0f, 248/255.0f, 250/255.0f, 255/255.0f};
  cTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  cTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
  cTargetInfo.texture = sTexture;

  SDL_GPURenderPass* rPass = SDL_BeginGPURenderPass(cmd, &cTargetInfo, 1, NULL);
  SDL_BindGPUGraphicsPipeline(rPass, gPipeline);
  SDL_GPUBufferBinding bufferBindings[1];
  bufferBindings[0].buffer = vBuffer;
  bufferBindings[0].offset = 0;
  SDL_BindGPUVertexBuffers(rPass, 0, bufferBindings, 1);

  SDL_DrawGPUPrimitives(
    rPass,
    vertexCount,
    1,
    0,
    0
  );
  SDL_EndGPURenderPass(rPass);
  SDL_SubmitGPUCommandBuffer(cmd);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  (void)appstate;
  (void)event;

  if(event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  (void)appstate;
  (void)result;

  SDL_ReleaseGPUBuffer(device, vBuffer);
  SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
  SDL_ReleaseGPUGraphicsPipeline(device, gPipeline);

  SDL_DestroyGPUDevice(device);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
