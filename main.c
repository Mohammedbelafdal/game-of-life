#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define GRID_WIDTH 300
#define GRID_HEIGHT 200

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  _Bool quit;
  _Bool enable_simulation;
  _Bool grid[GRID_WIDTH][GRID_HEIGHT];
  unsigned long long number_of_steps;
  unsigned long long number_of_cells;
} settings, settings;
settings setting;
void signalHandler(int signal) {
  switch (signal) {
  case SIGUSR1:
  case SIGTERM:
    abort();
  default:
    setting.quit = true;
    break;
  }
};
void Init() {
  setting.quit = false;
  setting.number_of_steps = 0;
  signal(SIGKILL, signalHandler);
  signal(SIGSEGV, signalHandler);
  signal(SIGUSR1, signalHandler);
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    puts("error");
    raise(SIGUSR1);
  }
  SDL_DisplayMode DM;
  SDL_GetDisplayMode(0, 0, &DM);
  setting.window = SDL_CreateWindow("game of life", // creates a window
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, DM.w, DM.h, 0);
  setting.renderer =
      SDL_CreateRenderer(setting.window, -1, SDL_RENDERER_ACCELERATED);
  memset(setting.grid, 0, GRID_HEIGHT * GRID_WIDTH);
  // initial seed
  for (int x = 0; x < GRID_WIDTH; x++) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
      setting.grid[x][y] = rand() < ((double)RAND_MAX) / 2.;
    }
  }

  SDL_RenderSetScale(setting.renderer, (float)DM.w / (float)GRID_WIDTH,
                     (float)DM.h / (float)GRID_HEIGHT);
}
void Render() {
  SDL_SetRenderDrawColor(setting.renderer, 0, 0, 0, 255);
  SDL_RenderClear(setting.renderer);
  SDL_SetRenderDrawColor(setting.renderer, 255, 255, 255, 255);
  int number_of_cells = 0;
  for (int x = 0; x < GRID_WIDTH; x++) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
      if (setting.grid[x][y] == 1) {
        SDL_RenderDrawPoint(setting.renderer, x, GRID_HEIGHT - y);
        number_of_cells ++;
      }
    }
  }
  setting.number_of_cells=number_of_cells;
  // printf("number of cells %d\n",number_of_cells);
  SDL_RenderPresent(setting.renderer);
}
void Compute() {
  _Bool new_grid[GRID_WIDTH][GRID_HEIGHT] = {0};
  int number_of_neighbors = 0;
  if (setting.enable_simulation == true) {
    for (int x = 1; x < GRID_WIDTH - 1; x++) {
      for (int y = 1; y < GRID_HEIGHT - 1; y++) {
        number_of_neighbors = 0;
        number_of_neighbors += setting.grid[x - 1][y - 1];
        number_of_neighbors += setting.grid[x][y - 1];
        number_of_neighbors += setting.grid[x + 1][y - 1];
        number_of_neighbors += setting.grid[x - 1][y];
        number_of_neighbors += setting.grid[x + 1][y];
        number_of_neighbors += setting.grid[x - 1][y + 1];
        number_of_neighbors += setting.grid[x][y + 1];
        number_of_neighbors += setting.grid[x + 1][y + 1];
        // underpopulation
        if (number_of_neighbors < 2)
          new_grid[x][y] = 0;
        // overpopulation
        if (number_of_neighbors > 3)
          new_grid[x][y] = 0;
        // survival
        if (number_of_neighbors == 2 || number_of_neighbors == 3)
          new_grid[x][y] = setting.grid[x][y];
        // reproduction
        if (number_of_neighbors == 3)
          new_grid[x][y] = 1;
      }
    }
    memcpy(setting.grid, new_grid, GRID_HEIGHT * GRID_WIDTH);
  }
  setting.number_of_steps++;
}
void ManageInputs(SDL_Event *event) {
  const Uint8 *state;
  while (SDL_PollEvent(event)) {
    switch (event->type) {
    case SDL_QUIT:
      setting.quit = true;
      break;
    case SDL_KEYDOWN:
      state = SDL_GetKeyboardState(NULL);
      if (state[SDL_SCANCODE_ESCAPE])
        setting.quit = true;
      if (state[SDL_SCANCODE_P])
      // pause simulation
      {
        puts("simulation paused :");
        setting.enable_simulation = false;
        printf("%llu steps, %llu living cells\n", setting.number_of_steps,setting.number_of_cells);
      }
      if (state[SDL_SCANCODE_R])
      // pause simulation
      {
        puts("simulation resumed\n");
        setting.enable_simulation = true;
      }

    default:
      break;
    }
  }
}

int main() {
  Init();
  SDL_Event event;
  while (setting.quit == false) {
    Render();
    Compute();
    ManageInputs(&event);
    SDL_Delay(1000. / 20.);// 20 steps per second desired
  }
  SDL_DestroyWindow(setting.window);
  SDL_Quit();
  return 0;
}