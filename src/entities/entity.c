#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../../include/debug.h"
#include "../../include/game.h"
#include "../../include/entities/entity.h"
#include "../../include/components/component.h"
#include "../../include/managers/assetstack.h"
#include "../../include/rendering/renderertemplates.h"

/**
 * Default deleted component call for all entities.
 */
void deleted(void *e)
{
    Entity *entity = (Entity *)e;
    entity->remove = true;
}

/**
 * Default render call for all entities.
 */
void render(void *e)
{
    Entity *entity = (Entity *)e;
    render_texture(entity->textures[0], &entity->position);
}

/**
 * Initialize an entity with a default size, location and with provided texture and sound.
 */
bool init_entity(Entity **e, const char *texture, const char *sound)
{
    Entity *entity = (Entity *)malloc(sizeof(Entity));

    // Initialize all components as null.
    for (int i = 0; i < COMPONENT_TOTAL; i++)
        entity->components[i].call = NULL;

    // Grab assets.
    if (texture != NULL)
    {
        RegisteredAsset *asset = (RegisteredAsset *)get_asset_by_ref(texture, 0);
        if (asset == NULL)
        {
            ERROR_LOG("Unable to find asset for entity: %s\n", texture);
            return false;
        }
        entity->textures[0] = asset->pointer.texture;
    }

    if (sound != NULL)
    {
        RegisteredAsset *asset = (RegisteredAsset *)get_asset_by_ref(sound, 0);
        if (asset == NULL)
        {
            ERROR_LOG("Unable to find asset for entity: %s\n", sound);
            return false;
        }
        entity->sounds[0] = asset->pointer.sound;
    }

    // Set components.
    entity->remove = false;
    entity->components[Deleted].call = &deleted;
    entity->components[Render].call = &render;
    *e = entity;
    return true;
}

/**
 * Returns true if the entity has queried component.
 */
bool has_component(Entity *e, ComponentType c)
{
    return !(e->components[c].call == NULL);
}
