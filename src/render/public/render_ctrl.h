#ifndef _RENDER_CTRL_H_
#define _RENDER_CTRL_H_

#include "../../lib/public/queue.h"
#include "../../lib/public/stalloc.h"

#include <stddef.h>

#include <SDL_mutex.h>
#include <SDL_thread.h>

struct frustum;
struct tile_desc;
struct map;

enum render_info {
    RENDER_INFO_VENDOR,
    RENDER_INFO_RENDERER,
    RENDER_INFO_VERSION,
    RENDER_INFO_SL_VERSION,
};

struct render_init_arg {
    SDL_Window *in_window;
    int in_width;
    int in_height;
    bool out_success;
};

struct render_sync_state {
    /* The render thread owns the data pointed to by 'arg' until
     * signalling the first 'done'. */
    struct render_init_arg *arg;
    /* The start flag is set by the main thread when the render
     * thread is allowed to start processing commands.
     * The quit flag is set by the main thread when the render
     * thread should exit. */
    bool start;
    bool quit;
    SDL_mutex *sq_lock;
    SDL_cond *sq_cond;
    /* The done flag is set by the render thread when it is done
     * procesing commands for the current frame. */
    bool done;
    SDL_mutex *done_lock;
    SDL_cond *done_cond;
    /* Flag to specify if the framebuffer should be presented on
     * the screen after all commands are executed */
    bool swap_buffers;
};

#define MAX_ARGS 8

struct rcmd {
    void (*func)();
    size_t nargs;
    void *args[MAX_ARGS];
};

QUEUE_TYPE(rcmd, struct rcmd)
QUEUE_IMPL(static inline, rcmd, struct rcmd)

struct render_workspace {
    /* Stack allocator for storing all the data/arguments associated
     * with the commands */
    struct memstack args;
    queue_rcmd_t commands;
};

bool R_Init(const char *base_path);
SDL_Thread *R_Run(struct render_sync_state *rstate);
/* Must be set up before creating the window */
void R_InitAttributes(void);

void *R_PushArg(const void *src, size_t size);
void R_PushCmd(struct rcmd cmd);

bool R_InitWS(struct render_workspace *ws);
void R_DestroyWS(struct render_workspace *ws);
void R_ClearWS(struct render_workspace *ws);

const char *R_GetInfo(enum render_info attr);

/* Shadows */
void R_LightFrustum(vec3_t light_pos, vec3_t cam_pos, vec3_t cam_dir, struct frustum *out);

/* Tile */
int R_TileGetTriMesh(const struct map *map, struct tile_desc *td, mat4x4_t *model, vec3_t out[]);

/* UI */
int R_UI_GetFontTexID(void);

#endif /* _RENDER_CTRL_H_ */
