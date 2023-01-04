#include "timer_events.h"
#include "../event.h"
#include "public/game.h"

#include <SDL.h>
#include <assert.h>
#include <math.h>

#define TIMER_INTERVAL (1000.0f / 60.0f)

/*****************************************************************************/
/* STATIC VARIABLES                                                          */
/*****************************************************************************/

static unsigned long long s_num_60hz_ticks;
static SDL_TimerID s_60hz_timer;

/*****************************************************************************/
/* STATIC FUNCTIONS                                                          */
/*****************************************************************************/

/* Timer callback gets called from another thread. In the callback, push
 * a user event (threadsafe) with the code '0' and handle the 'EVENT_60HZ_TICK'
 * event in the main thread.
 */
static uint32_t timer_callback(uint32_t interval, void *param) {
  static double s_error = 0.0f;
  s_error += (TIMER_INTERVAL - interval);

  double intpart, fracpart;
  fracpart = modf(s_error, &intpart);
  (void)fracpart;

  s_error -= intpart;
  interval = ((int)TIMER_INTERVAL) + intpart;

  SDL_Event event = (SDL_Event){
      .type = SDL_USEREVENT,
      .user =
          (SDL_UserEvent){
              .type = SDL_USEREVENT,
              .code = 0,
              .data1 = NULL,
              .data2 = NULL,
          },
  };

  SDL_PushEvent(&event);
  return interval;
}

static void timer_60hz_handler(void *unused1, void *unused2) {
  s_num_60hz_ticks++;

  if (s_num_60hz_ticks % 2 == 0)
    E_Global_Notify(EVENT_30HZ_TICK, NULL, ES_ENGINE);

  if (s_num_60hz_ticks % 3 == 0)
    E_Global_Notify(EVENT_20HZ_TICK, NULL, ES_ENGINE);

  if (s_num_60hz_ticks % 4 == 0)
    E_Global_Notify(EVENT_15HZ_TICK, NULL, ES_ENGINE);

  if (s_num_60hz_ticks % 6 == 0)
    E_Global_Notify(EVENT_10HZ_TICK, NULL, ES_ENGINE);

  if (s_num_60hz_ticks % 60 == 0)
    E_Global_Notify(EVENT_1HZ_TICK, NULL, ES_ENGINE);
}

/*****************************************************************************/
/* EXTERN FUNCTIONS                                                          */
/*****************************************************************************/

bool G_Timer_Init(void) {
  s_60hz_timer = SDL_AddTimer(TIMER_INTERVAL, timer_callback, NULL);
  if (0 == s_60hz_timer)
    return false;

  /* We will still generate timer events while the simulation is paused.
   * Most handlers should be masked out, however. */
  E_Global_Register(EVENT_60HZ_TICK, timer_60hz_handler, NULL,
                    G_RUNNING | G_PAUSED_UI_RUNNING | G_PAUSED_FULL);
  return true;
}

void G_Timer_Shutdown(void) {
  E_Global_Unregister(EVENT_60HZ_TICK, timer_60hz_handler);
  SDL_RemoveTimer(s_60hz_timer);
}
