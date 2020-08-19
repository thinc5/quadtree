#include <SDL2/SDL.h>

#include <stdio.h>

#include "../../include/debug.h"
#include "../../include/util/timer.h"
#include "../../include/util/framerate.h"
#include "../../include/config.h"

/**
 * Initialise the timer using some global constants defined in config.h.
 */
FrameRateManager init_fps(void)
{
    FrameRateManager f = (FrameRateManager){
        .capped = FPS_CAPPED,
        .cappedFPS = FRAME_CAP,
        .frames = 0,
        .seconds = 0,
        .timer = init_timer(),
        .second = init_timer(),
    };
    start_timer(&f.timer);
    start_timer(&f.second);
    return f;
}

/**
 * Maintain capped framerate.
 */
void cap_fps(FrameRateManager *f)
{
    if (f->capped)
    {
        int cap = (1000 / f->cappedFPS);
        // Have we "finished" early?
        if (!time_elapsed(&f->timer, cap))
            SDL_Delay(cap - (SDL_GetTicks() - f->timer.startTime));

        f->timer.startTime = SDL_GetTicks();
    }

    // Have we passed one second? What is the fps?
    if (time_elapsed(&f->second, 1000))
    {
        f->seconds++;
        f->second.startTime = SDL_GetTicks();
        if (f->seconds > 60000)
        {
            f->seconds /= 2;
            f->frames /= 2;
        }
    }
    f->frames++;
}

/**
 * Get the current average fps.
 */
int get_fps(FrameRateManager *f)
{
    if (f->frames > 0 && f->seconds > 0)
        return f->frames / f->seconds;

    return 0;
}
