#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

int main() {
  SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");

  if(!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed");
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Test Window", 720, 480, 0);

  if(!window) {
    SDL_Log("Error creating window %s ", SDL_GetError());
    return 2;
  };

  SDL_ShowWindow(window);

  SDL_Event event;
  bool running = true;
  Uint32 startTime = SDL_GetTicks();

  while(running) {
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }
    if(SDL_GetTicks() - startTime > 4000) {
      running = false;
    }
    SDL_Delay(16);
  }
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
