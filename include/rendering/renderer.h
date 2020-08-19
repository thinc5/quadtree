#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

#include "../game.h"
#include "../scenes/scene.h"

/**
 * Render all renderable entities.
 */
void render_entities(Scene *scene);

#endif
