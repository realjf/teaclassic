#ifndef _MAIN_H_
#define _MAIN_H_

#include <SDL.h>
#include <assert.h>
#include <stdbool.h>

extern const char *g_basepath;          /* readonly */
extern unsigned g_last_frame_ms;        /* readonly */
extern unsigned long g_frame_idx;       /* readonly */
extern SDL_threadID g_main_thread_id;   /* readonly */
extern SDL_threadID g_render_thread_id; /* readonly */

#define ASSERT_IN_RENDER_THREAD() assert(SDL_ThreadID() == g_render_thread_id)

#define ASSERT_IN_MAIN_THREAD() assert(SDL_ThreadID() == g_main_thread_id)

enum tc_window_flags {

  TC_WF_FULLSCREEN = SDL_WINDOW_FULLSCREEN | SDL_WINDOW_INPUT_GRABBED,
  TC_WF_BORDERLESS_WIN = SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_GRABBED,
  TC_WF_WINDOW = SDL_WINDOW_INPUT_GRABBED,

};

int Engine_SetRes(int w, int h);
void Engine_SetDispMode(enum tc_window_flags wf);
void Engine_WinDrawableSize(int *out_w, int *out_h);
void Engine_LoadingScreen(void);
void Engine_EnableRendering(bool on);

/**
 * Execute all the currently queued render commands on the render thread.
 * Block until it completes. This is used during initialization only to execute
 * rendering code serially.
 */
void Engine_FlushRenderWorkQueue(void);
void Engine_WaitRenderWorkDone(void);
void Engine_ClearPendingEvents(void);
bool Engine_GetArg(const char *name, size_t maxout, char out[static maxout]);

#endif /* _MAIN_H_ */
