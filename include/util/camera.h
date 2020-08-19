#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>

#include <stdbool.h>

/**
 * Possible directions.
 */
typedef enum Quadrent
{
    TOPLEFT,
    TOPRIGHT,
    BOTLEFT,
    BOTRIGHT,
    QUADRENTS
} Quadrent;

/**
 * Given an SDL_Rect and the desired scaling (0.0f is the centre, 1.0f the edge) modify
 * the x and y coords to match the desired location.
 */
SDL_Rect transform_rect(SDL_Rect within, float x, float y, float width, float height);

/**
 * Given an SDL_Rect and the desired scaling (0.0f is the centre, 1.0f the edge) modify
 * the x and y coords to match the desired location while ensuring that you end up with a
 * square that fits within the bounds provided..
 */
SDL_Rect transform_right_angle_square(SDL_Rect within, float x, float y, float width, float height);

/**
 * Given an SDL_Rect and the desired scaling (0.0f is the centre, 1.0f the edge) modify
 * the x and y coords to create a right angled rectangle.
 */
SDL_Rect transform_right_angle_rect(SDL_Rect within, float x, float y, float width, float height,
                                    int unitX, int unitY);

/**
 * Check if provided x and y coordinates are inside of provided rectangle.
 */
bool is_collision(int x, int y, SDL_Rect position);

/**
 * Check if provided rects overlap.
 */
bool is_overlap(SDL_Rect dest, SDL_Rect position);

/**
 * Check if provided rect is entirely within the second rect.
 */
bool is_inside(SDL_Rect container, SDL_Rect within);

/**
 * Get the center of a rectangle as a SDL_Point.
 */
SDL_Point get_rect_centre(SDL_Rect rect);

/**
 * Returns true if the point is within the provided rectangle.
 */
bool is_point_inside(SDL_Rect within, SDL_Point point);

Quadrent get_dir(SDL_Point centre, SDL_Point point);

#endif
