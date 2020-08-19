#ifndef QUADTREE_H
#define QUADTREE_H

#include <SDL2/SDL.h>

#include <stdbool.h>

#include "../util/camera.h"
#include "../entities/entity.h"

/**
 * The node of the tree.
 */
typedef struct QuadTreeNode
{
    // The bounds of this node.
    SDL_Rect bounds;
    // The entity stored in this node.
    Entity *entity;
    // The children of this node.
    struct QuadTreeNode *children[QUADRENTS];
    // The parent of this node, null if the root node.
    struct QuadTreeNode *parent;
} QuadTreeNode;

/**
 * The quad tree.
 */
typedef struct QuadTree
{
    // The root.
    QuadTreeNode *root;
    // Number of nodes.
    uint16_t size;
} QuadTree;

/**
 * Initialize the new QuadTree node.
 */
void quad_init_tree(QuadTree *quad, SDL_Rect bounds);

/**
 * Free quad tree.
 */
void quad_free_tree(QuadTree *quad);

/**
 * Returns the pointer to the largest node that falls within the provided
 * rectangle.
 */
QuadTreeNode *quad_find_node(QuadTreeNode *node, SDL_Rect point);

/**
 * Returns the pointer to stored entity on success, and NULL
 * if no entity was found.
 */
QuadTreeNode *quad_find_entity(QuadTreeNode *node, SDL_Rect point);

/**
 * Insert an entity into the quad tree.
 */
bool quad_insert_entity(QuadTreeNode *node, Entity *entity);

/**
 * Remove an entity from the quad tree.
 */
bool quad_remove_entity(QuadTreeNode *node, SDL_Rect point);

#endif
