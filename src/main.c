#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include "../include/config.h"
#include "../include/debug.h"
#include "../include/util/os.h"
#include "../include/util/framerate.h"

#include "../include/managers/asset.h"
#include "../include/managers/assetstack.h"
#include "../include/managers/eventmanager.h"

#include "../include/rendering/renderer.h"
#include "../include/rendering/renderertemplates.h"

#include "../include/game.h"
#include "../include/scenes/scene.h"

GameData gameData;

// Font to be used to render debug messages.
static RegisteredAsset *fnt;

/**
 * Debug function to show location of mouse.
 */
static void dbg(TTF_Font *fnt)
{
    char fps[100];
    sprintf(fps, "FPS: %3d", get_fps(&gameData.fps));
    char camera[100];
    sprintf(camera, "View: (%4d, %4d, %4d, %4d) Camera: (%4d, %4d, %4d, %4d)",
            gameData.view.x, gameData.view.y, gameData.view.w, gameData.view.h,
            gameData.camera.x, gameData.camera.y, gameData.camera.w, gameData.camera.h);
    int x, y;
    SDL_GetMouseState(&x, &y);
    char mouse[100];
    sprintf(mouse, "Mouse Position: x %4d y %4d", x, y);
    char entities[100];
    sprintf(entities, "Entities: %5d", gameData.scene->entities.current);

    // Render.
    render_debug_message(fnt, fps, 0);
    render_debug_message(fnt, camera, 1);
    render_debug_message(fnt, mouse, 2);
    render_debug_message(fnt, entities, 3);
}

/**
 *  Initialize SDL components.
 */
static bool init_modules(void)
{
    // Start SDL and components.
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        ERROR_LOG("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    if (TTF_Init() != 0)
    {
        ERROR_LOG("Unable to initialize SDL_ttf\n");
        return false;
    }
    return true;
}

/**
 * Quit SDL components.
 */
static void quit_modules(void)
{
    TTF_Quit();
    SDL_Quit();
}

/**
 * Handle the user events.
 */
static void handle_events(void)
{
    while (SDL_PollEvent(&gameData.event))
    {
        if (gameData.event.type == SDL_QUIT)
        {
            gameData.status = CLOSING;
            return;
        }
        gameData.currentScene->event_handler();
    }
}

/**
  * Update game state.
  */
static void update_state(void)
{
    // Do not process any entities whilst we are loading scenes.
    if (gameData.status == LOADING)
        return;

    for (int i = 0; i < gameData.currentScene->entities.current; i++)
    {
        if (has_component(gameData.currentScene->entities.entities[i], OnTick))
            gameData.currentScene->entities.entities[i]->components[OnTick].call(&gameData.currentScene->entities.entities[i]);
    }
    // Remove all entities marked for deletion.
    clean_entities(&gameData.currentScene->entities);
}

/**
 * Render game state.
 */
static void render_state(void)
{
    // Clear the 'canvas'.
    SDL_RenderClear(gameData.renderer);

    // Render stuff.
    render_background(gameData.currentScene);
    render_entities(gameData.currentScene);

    if (gameData.debug)
    {
        // Render the camera view box.
        render_rectangle(&gameData.camera,
                         (SDL_Color){.r = 255, .g = 0, .b = 0, .a = 255},
                         false);

        dbg(fnt->pointer.font);
    }

    // Present the frame.
    SDL_RenderPresent(gameData.renderer);
}

/**
 * Entry point for the engine.
 */
int main(int argc, char **argv)
{
    INFO_LOG("Launching: \"%s\"\n", WINDOW_TITLE);
    // Change working directory to the directory of the executable
    // (allows exe to be used from any path).
    set_dir();
    // Start all SDL components and load the game.
    if (!init_modules() || !init_game(&gameData))
    {
        ERROR_LOG("Unable to initialize game modules.\n");
        return 1;
    }

    // Seed rand() to make it seem actually random.
    srand(time(NULL));

    // Font to be used for debug rendering.
    fnt = get_asset_by_ref("ssp-regular.otf", 0);

    while (gameData.status != CLOSING)
    {
        switch (gameData.status)
        {
        case RUNNING:
            // Handle user events.
            handle_events();
            // Update state.
            update_state();
            // Render state.
            render_state();
            // Wait if we have finished too soon (capping fps).
            cap_fps(&gameData.fps);
            break;
        case LOADING:
            break;
        default:
            break;
        }
    }

    // Clean up.
    free_game(&gameData);
    quit_modules();
    INFO_LOG("\"%s\" terminated\n", WINDOW_TITLE);
    return 0;
}
