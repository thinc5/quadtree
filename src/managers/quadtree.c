#include <SDL2/SDL.h>

#include <stdlib.h>
#include <stdbool.h>

#include "../../include/managers/quadtree.h"
#include "../../include/entities/entity.h"
#include "../../include/util/camera.h"

#include "../../include/debug.h"

// ---------------- Helper functions ----------------

/**
 * Create a node.
 */
static QuadTreeNode *quad_init_node(QuadTreeNode *parent, SDL_Rect bounds)
{
    // Allocate the root node.
    QuadTreeNode *node = (QuadTreeNode *)malloc(sizeof(QuadTreeNode));
    node->parent = parent;

    // Children nodes.
    for (Quadrent q = 0; q < QUADRENTS; q++)
        node->children[q] = NULL;

    node->entity = NULL;

    // Set the bounds.
    node->bounds = bounds;
    return node;
}

/**
 * Free a node.
 */
static void quad_free_node(QuadTreeNode *node)
{
    for (Quadrent q = 0; q < QUADRENTS; q++)
    {
        if (node->children[q])
            quad_free_node(node->children[q]);
    }

    free(node);
}

/**
 * Is this node a leaf? (No children, degree 0).
 */
static inline bool quad_is_leaf(QuadTreeNode *node)
{
    return node->children[TOPLEFT] == NULL &&
           node->children[TOPRIGHT] == NULL &&
           node->children[BOTLEFT] == NULL &&
           node->children[BOTRIGHT] == NULL;
}

/**
 * Is this node a leaf and occupied?
 */
static inline bool quad_is_occupied(QuadTreeNode *node)
{
    if (!node || !node->entity)
        return false;

    return quad_is_leaf(node);
}

/**
 * Turn a leaf into a branch and relocate the entity.
 */
static void quad_subdivide(QuadTreeNode *node)
{
    SDL_Point centre = get_rect_centre(node->bounds);
    SDL_Rect bounds = node->bounds;

    // Create the children.
    node->children[TOPLEFT] =
        quad_init_node(node,
                       (SDL_Rect){.x = bounds.x, .y = bounds.y, .w = bounds.w / 2, .h = bounds.h / 2});
    node->children[TOPRIGHT] = quad_init_node(node, (SDL_Rect){.x = bounds.x + (bounds.w / 2), .y = node->bounds.y, .w = bounds.w / 2, .h = bounds.h / 2});
    node->children[BOTLEFT] = quad_init_node(node, (SDL_Rect){.x = bounds.x, .y = node->bounds.y + (bounds.h / 2), .w = bounds.w / 2, .h = bounds.h / 2});
    node->children[BOTRIGHT] = quad_init_node(node, (SDL_Rect){.x = bounds.x + (bounds.w / 2), .y = node->bounds.y + (bounds.h / 2), .w = bounds.w / 2, .h = bounds.h / 2});

    // Push the old entity down. if there is one.
    if (node && node->entity)
    {
        SDL_Point point = get_rect_centre(node->entity->position);
        Quadrent q = get_dir(centre, point);
        node->children[q]->entity = node->entity;
        node->entity = NULL;
    }
}

/**
 * Restore a node when there is only one or no entities.
 * Turns branch into a leaf.
 */
static void quad_restore(QuadTreeNode *node)
{
    Entity *j = NULL;
    // Find our entity to pull up.
    for (int q = 0; q < QUADRENTS; q++)
    {
        if (node->children[q]->entity != NULL)
        {
            if (j != NULL)
            {
                return;
            }
            j = node->children[q]->entity;
        }
    }
    // Free the child nodes.
    for (Quadrent q = 0; q < QUADRENTS; q++)
    {
        quad_free_node(node->children[q]);
        node->children[q] = NULL;
    }
    // Set the entity.
    node->entity = j;
}

// ---------------- Main functions ----------------

/**
 * Initialize the quad tree node.
 */
void quad_init_tree(QuadTree *quad, SDL_Rect bounds)
{
    // Size of the quad tree.
    quad->size = 0;

    // Initialize the root node.
    quad->root = (QuadTreeNode *)malloc(sizeof(QuadTreeNode));
    quad->root->parent = NULL;

    // Children nodes.
    for (Quadrent q = 0; q < QUADRENTS; q++)
        quad->root->children[q] = NULL;

    quad->root->entity = NULL;

    // Set the bounds.
    quad->root->bounds = bounds;
}

/**
 * Free quad tree.
 */
void quad_free_tree(QuadTree *quad)
{
    quad_free_node(quad->root);
}

/**
 * Returns the pointer to the largest node that falls within the provided
 * rectangle.
 */
QuadTreeNode *quad_find_node(QuadTreeNode *node, SDL_Rect view)
{
    // Does this node exist?
    if (!node)
        return NULL;

    DEBUG_LOG("Checking node %p\n", (void *)node);

    // Is this node inside? If so we return the parent.
    if (is_inside(view, node->bounds))
    {
        DEBUG_LOG("quad (%d %d %d %d) is outside view %d %d %d %d\n", node->bounds.x, node->bounds.y,
                  node->bounds.w, node->bounds.h, view.x, view.y, view.w, view.h);
        return node->parent;
    }

    // Check our children to see if any contain the view.
    for (Quadrent q = 0; q < QUADRENTS; q++)
    {
        QuadTreeNode *selected = quad_find_node(node->children[q], view);
        if (selected)
            return selected;
    }

    return NULL;
}

/**
 * Does this node have any children?
 * Returns the pointer to stored entity on success, and NULL
 * if no entity was found.
 */
QuadTreeNode *quad_find_entity(QuadTreeNode *node, SDL_Rect point)
{
    // Does this node exist?
    if (!node)
        return NULL;

    // Is this a leaf?
    if (quad_is_occupied(node))
        return node;

    // Where do we search next?
    SDL_Point centre = get_rect_centre(node->bounds);
    SDL_Point p = get_rect_centre(point);

    // Get direction.
    Quadrent q = get_dir(centre, p);
    switch (q)
    {
    case QUADRENTS:
        // No direction.
        return NULL;
    case TOPLEFT:
        if (node->children[TOPLEFT] != NULL)
            quad_find_entity(node->children[TOPLEFT], point);
        break;
    case TOPRIGHT:
        if (node->children[TOPRIGHT] != NULL)
            quad_find_entity(node->children[TOPRIGHT], point);
        break;
    case BOTLEFT:
        if (node->children[BOTLEFT] != NULL)
            quad_find_entity(node->children[BOTLEFT], point);
        break;
    case BOTRIGHT:
        if (node->children[BOTRIGHT] != NULL)
            quad_find_entity(node->children[BOTRIGHT], point);
        break;
    }
    return NULL;
}

/**
 * Insert an entity into the quad tree.
 */
bool quad_insert_entity(QuadTreeNode *node, Entity *entity)
{
    if (!node)
        ERROR_LOG("Called on a null node!\n");

    if (!is_point_inside(node->bounds, get_rect_centre(entity->position)))
        return false;

    // Do we have space in the current node to add entity?
    if (quad_is_leaf(node) && node->entity == NULL)
    {
        // Place entity and return.
        node->entity = entity;
        return true;
    }
    // Are we on a branch or a leaf?
    if (quad_is_leaf(node))
    {
        quad_subdivide(node);
        return quad_insert_entity(node, entity);
    }

    // We can try and find a place to put the entity.
    Quadrent q = get_dir(get_rect_centre(node->bounds),
                         get_rect_centre(entity->position));
    switch (q)
    {
    case TOPLEFT:
        if (!node->children[TOPLEFT]->entity)
            quad_init_node(node, (SDL_Rect){.x = node->bounds.x, .y = node->bounds.y, .w = node->bounds.w / 2, .h = node->bounds.h / 2});

        return quad_insert_entity(node->children[TOPLEFT], entity);
    case TOPRIGHT:
        if (!node->children[TOPRIGHT]->entity)
            quad_init_node(node, (SDL_Rect){.x = node->bounds.x + (node->bounds.w / 2), .y = node->bounds.y, .w = node->bounds.w / 2, .h = node->bounds.h / 2});

        return quad_insert_entity(node->children[TOPRIGHT], entity);
    case BOTLEFT:
        if (!node->children[BOTLEFT]->entity)
            quad_init_node(node, (SDL_Rect){.x = node->bounds.x, .y = node->bounds.y + (node->bounds.h / 2), .w = node->bounds.w / 2, .h = node->bounds.h / 2});

        return quad_insert_entity(node->children[BOTLEFT], entity);
    case BOTRIGHT:
        if (!node->children[BOTRIGHT]->entity)
            quad_init_node(node, (SDL_Rect){.x = node->bounds.x + (node->bounds.w / 2), .y = node->bounds.y + (node->bounds.h / 2), .w = node->bounds.w / 2, .h = node->bounds.h / 2});

        return quad_insert_entity(node->children[BOTRIGHT], entity);
    default:
        break;
    }

    return false;
}

/**
 * Remove an entity from the quad tree.
 */
bool quad_remove_entity(QuadTreeNode *node, SDL_Rect point)
{
    QuadTreeNode *found = quad_find_entity(node, point);
    if (!found)
        return false;

    QuadTreeNode *foundParent = found->parent;
    // Mark the entity for cleanup.
    // If it has already not been marked for removal.
    if (found->entity)
        found->entity->remove = true;

    // Clean up the node.
    quad_free_node(node);

    // Do we need to clean up the parent node?
    if (foundParent == NULL)
        return true;

    int children = 0;
    for (Quadrent q = 0; q < QUADRENTS; q++)
    {
        if (foundParent->children[q] != NULL)
            children++;
    }
    // Need to restore the nodes.
    if (children < 2)
        quad_restore(foundParent);

    return true;
}
