#include <SDL2/SDL.h>

#include <stdlib.h>
#include <stdbool.h>

#include "../include/debug.h"
#include "../include/config.h"
#include "../include/game.h"
#include "../include/util/framerate.h"
#include "../include/scenes/scene.h"
#include "../include/scenes/quadtest.h"

/**
 * Initialize game and it's required components.
 */
bool init_game(GameData *gameData)
{
    // Game is running?
    gameData->status = RUNNING;
    gameData->debug = false;

    // Create the window.
    gameData->window = SDL_CreateWindow(WINDOW_TITLE,
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT, 0); //SDL_WINDOW_RESIZABLE);

    // Framerate manager.
    gameData->fps = init_fps();

    // Create the asset stack
    init_asset_stack(&gameData->assets);

    // Camera
    gameData->view = (SDL_Rect){.x = 0, .y = 0};
    SDL_GetWindowSize(gameData->window, &gameData->view.w,
                      &gameData->view.h);
    gameData->camera = gameData->view;

    // Scenes.
    gameData->scene = (Scene *)malloc(sizeof(Scene));
    init_quadtest_scene();
    gameData->currentScene = gameData->scene;

    // Create the renderer.
    gameData->renderer = SDL_CreateRenderer(gameData->window, -1,
                                            SDL_RENDERER_ACCELERATED);

    // Set default background colour.
    SDL_SetRenderDrawColor(gameData->renderer, 0, 0, 0, 255);
    SDL_RenderSetLogicalSize(gameData->renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetRenderDrawBlendMode(gameData->renderer, SDL_BLENDMODE_BLEND);
    return true;
}

/**
 * Free game data.
 */
void free_game(GameData *gameData)
{
    DEBUG_LOG("free_game()\n");
    // Free renderer and window.
    SDL_DestroyRenderer(gameData->renderer);
    SDL_DestroyWindow(gameData->window);

    // Free scenes.
    if (gameData->scene != NULL)
    {
        free_scene(gameData->scene);
        free(gameData->scene);
        gameData->scene = NULL;
        DEBUG_LOG("Freed scene\n");
    }
    gameData->currentScene = NULL;
}
