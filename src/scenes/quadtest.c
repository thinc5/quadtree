#include <stdlib.h>
#include <math.h>

#include "../../include/debug.h"
#include "../../include/managers/quadtree.h"
#include "../../include/game.h"
#include "../../include/entities/entity.h"
#include "../../include/entities/state.h"
#include "../../include/entities/node.h"

/**
 * Click handler for the quadtree test.
 */
static void click_handler(GameData *gameData, bool *right_down)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    if (gameData->event.button.button == SDL_BUTTON_LEFT)
    {
        // Fetch the component via the quadtree.
        QuadTreeNode *found = quad_find_entity(gameData->scene->spacial.root,
                                               (SDL_Rect){.x = x, .y = y});
        if (!found)
            return;

        if (!has_component(found->entity, LeftClicked))
            return;

        DEBUG_LOG("Click at x: %d, y:%d\n", x, y);
        found->entity->components[LeftClicked].call(found->entity);
        return;
    }
    if (gameData->event.button.button == SDL_BUTTON_RIGHT)
    {
        if (gameData->event.type == SDL_MOUSEBUTTONDOWN)
            *right_down = true;
        if (gameData->event.type == SDL_MOUSEBUTTONUP)
            *right_down = false;
    }
}

/**
 * Key press handler for the quadtree test scene.
 */
static void key_handler(GameData *gameData, bool *alt_down)
{
    switch (gameData->event.key.keysym.sym)
    {
    case SDLK_LALT:
    case SDLK_RALT:
        if (gameData->event.type == SDL_KEYDOWN)
        {
            DEBUG_LOG("ALT PRESSED!!!");
            *alt_down = true;
        }
        if (gameData->event.type == SDL_KEYUP)
        {
            *alt_down = false;
            DEBUG_LOG("ALT RELEASED!!!");
        }
        break;
    case SDLK_F4:
        if (*alt_down)
            gameData->status = CLOSING;
        break;
    case SDLK_F5:
        if (gameData->event.type == SDL_KEYDOWN)
            gameData->debug = !gameData->debug;
        break;
    default:
        break;
    }
}

/**
 * Scroll wheel handler.
 */
static void scroll_handler(GameData *gameData)
{
    // Center of zoom in our zoom out.
    SDL_Point point;
    SDL_GetMouseState(&point.x, &point.y);

    // How much do we zoom in?
    float zoom_ratio = 0.05;

    if (gameData->event.wheel.y > 0)
    {
        SDL_Rect newCamera = {
            .x = gameData->camera.x + roundf(gameData->camera.w * (zoom_ratio / 4)),
            .y = gameData->camera.y + roundf(gameData->camera.h * (zoom_ratio / 4)),
            .w = gameData->camera.w - roundf(gameData->camera.w * (zoom_ratio / 2)),
            .h = gameData->camera.h - roundf(gameData->camera.h * (zoom_ratio / 2))};

        // Box too small?
        if (newCamera.w < 160 || newCamera.h < 80)
            return;

        // X and Y are increasing.
        newCamera.x = newCamera.x > gameData->view.w - 160 ? gameData->view.w - 160 : newCamera.x;
        newCamera.y = newCamera.y > gameData->view.h - 80 ? gameData->view.h - 80 : newCamera.y;

        // Set the new camera.
        gameData->camera = newCamera;
    }
    else if (gameData->event.wheel.y < 0)
    {
        SDL_Rect newCamera = {
            .x = gameData->camera.x - roundf(gameData->camera.w * (zoom_ratio / 4)),
            .y = gameData->camera.y - roundf(gameData->camera.h * (zoom_ratio / 4)),
            .w = gameData->camera.w + roundf(gameData->camera.w * (zoom_ratio / 2)),
            .h = gameData->camera.h + roundf(gameData->camera.h * (zoom_ratio / 2))};

        // X and Y are decreasing.
        newCamera.x = newCamera.x < gameData->view.x ? gameData->view.x : newCamera.x;
        newCamera.y = newCamera.y < gameData->view.y ? gameData->view.y : newCamera.y;

        // W and H are increasing.
        newCamera.w = newCamera.w > gameData->view.w ? gameData->view.w : newCamera.w;
        newCamera.h = newCamera.h > gameData->view.h ? gameData->view.h : newCamera.h;

        // Set the new camera.
        gameData->camera = newCamera;
    }
}

/**
 * Quadtree test scene event handler.
 */
static void event_handler(void)
{
    // Button down events are handled here.
    static Timer spawn_delay = {0};
    static bool alt_down = false;
    static bool right_down = false;

    switch (gameData.event.type)
    {
    case SDL_MOUSEWHEEL:
        scroll_handler(&gameData);
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        click_handler(&gameData, &right_down);
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        // Open the escape menu modal.
        if (gameData.event.key.keysym.sym == SDLK_ESCAPE)
        {
            gameData.status = CLOSING;
            return;
        }
        key_handler(&gameData, &alt_down);
        break;
    default:
        break;
    }

    // Spawn node.
    if (right_down && (time_elapsed(&spawn_delay, 50) || !spawn_delay.started))
    {
        // Spawn a new node.
        int x, y;
        SDL_GetMouseState(&x, &y);
        SDL_Rect pos;
        pos.x = x;
        pos.y = y;
        pos.w = 10;
        pos.h = 10;
        add_entity(&gameData.scene->entities, init_node, pos);
        // Reset timer.
        if (!spawn_delay.started)
        {
            spawn_delay = init_timer();
            start_timer(&spawn_delay);
        }
        spawn_delay.startTime = SDL_GetTicks();
    }
}

/**
 * Initialize the quadtree test scene.
 */
void init_quadtest_scene(void)
{
    if (!init_scene(gameData.scene))
    {
        gameData.scene = NULL;
        return;
    }

    // Load assets for the main menu.
    if (!push_asset_chunk(gameData.renderer, &gameData.assets,
                          "./res/quadtest.manifest"))
    {
        free_scene(gameData.scene);
        return;
    }

    // Init state.
    gameData.scene->state = (void *)malloc(sizeof(QuadTestState));
    // QuadTestState* state = (QuadTestState*)gameData.scene->state;

    // Set auxillary scene data.
    gameData.scene->event_handler = &event_handler;
}
