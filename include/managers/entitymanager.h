#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <stdbool.h>

#include "../entities/entity.h"
#include "../managers/assetstack.h"

/**
 * Stores Entities in 
 */
typedef struct EntityManager
{
    unsigned int current;
    unsigned int maximum;
    Entity **entities;
} EntityManager;

/**
 * Create new entity manager.
 */
bool init_entity_manager(EntityManager *entityManager);

/**
 * Add a new entity to the manager if there is space, if not allocate more
 * space for new entities.
 */
void add_entity(EntityManager *entityManager, Entity *(*init_entity)(void),
                SDL_Rect rect);

/**
 * Remove all entities that are marked to delete then shift all right of the
 * removed entity left to conserve space.
 */
void clean_entities(EntityManager *entityManager);

/**
 * Free entity manager.
 */
void free_entities(EntityManager *entityManager);

#endif
