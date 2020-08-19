#include "../../include/entities/entity.h"
#include "../../include/rendering/renderertemplates.h"

/**
 * Delete the node.
 */
static void destroy(void *e)
{
    Entity *entity = (Entity *)e;
    entity->components[Deleted].call();
}

/**
 * Render the node.
 */
static void render(void *e)
{
    Entity *entity = (Entity *)e;
    render_rectangle(&entity->position,
                     (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255}, true);
}

/**
 * Initializes the node entity.
 */
Entity *init_node(void)
{
    Entity *node = NULL;
    // Load cat assets make more flexable and specific later.
    if (!init_entity(&node, NULL, NULL))
        return NULL;

    node->components[LeftClicked].call = &destroy;
    node->components[Render].call = &render;
    return node;
}
