#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>
#include <string.h>

#include "../../include/debug.h"
#include "../../include/game.h"
#include "../../include/managers/quadtree.h"
#include "../../include/rendering/renderer.h"
#include "../../include/rendering/renderertemplates.h"
#include "../../include/scenes/scene.h"
#include "../../include/entities/entity.h"

/**
 * Render node if it is within the camera view..
 */
void render_node(QuadTreeNode *node)
{
    // Try the children.
    for (Quadrent q = 0; q < QUADRENTS; q++)
        if (node->children[q])
            render_node(node->children[q]);

    if (is_inside(gameData.camera, node->bounds) ||
        (gameData.debug && node->entity &&
         is_point_inside(gameData.camera,
                         get_rect_centre(node->entity->position))))
    {
        // Render entities.
        if (node->entity)
        {
            // Fill the box in!
            render_rectangle(&node->bounds,
                             (SDL_Color){.r = 0, .g = 0, .b = 255, .a = 127},
                             true);
            // Render said entity.
            if (has_component(node->entity, Render))
                node->entity->components[Render].call(node->entity);
        }
        render_rectangle(&node->bounds,
                         (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
                         false);
    }
}

/**
 * Render all entities within camera view.
 */
void render_entities(Scene *currentScene)
{
    render_node(gameData.scene->spacial.root);
}
