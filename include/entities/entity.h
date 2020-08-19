#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <stdbool.h>

#include "../managers/assetstack.h"
#include "../components/component.h"
#include "../util/timer.h"

/**
 * A simple entity, with a position, texture and movement function.
 */
typedef struct Entity
{
    SDL_Rect position;
    Component components[COMPONENT_TOTAL];
    // Assets.
    SDL_Texture *textures[4];
    Mix_Chunk *sounds[4];
    TTF_Font *font;
    Timer timers[4];
    // Mark the entity for collection?
    bool remove;
} Entity;

/**
 * Initialize an entity with a default size, location and with provided texture.
 */
bool init_entity(Entity **e, const char *texture, const char *sound);

/**
 * Returns true if the entity has queried component.
 */
bool has_component(Entity *e, ComponentType c);

#endif
