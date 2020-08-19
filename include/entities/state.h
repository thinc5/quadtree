#ifndef E_QUAD_STATE_H
#define E_QUAD_STATE_H

#include <SDL2/SDL.h>

#include <stdint.h>

#include "entity.h"

/**
 * The debug scene's state struct.
 */
typedef struct QuadTestState
{
    // Diagnostic statistics.
    uint32_t entities;
} QuadTestState;

Entity *init_quadtest_state(void);

#endif
