#ifndef _UI_H_
#define _UI_H_

#include "tc_math.h"
#include <stdbool.h>
#include <SDL.h>

struct nk_context;
struct nk_buffer;

struct rect {
    int x, y, w, h;
};

struct rgba {
    unsigned char r, g, b, a;
};

/* When the window is anchored to more than one point in a single dimension, the aspect ratio of the window
 * will change as the screen aspect ratio changes. For example, anchoring a full-screen window to the top,
 * bottom, left and right edges will ensure that it always takes up the entire screen regardless of resolution.
 * When a window is anchored to only one point in each dimension, the aspect ratio will stay constant. */
enum resize_opts {
    /* Distance between window's left edge and screen's left edge is constant */
    ANCHOR_X_LEFT = (1 << 0),
    /* Distance between window's right edge and screen's right edge is constant */
    ANCHOR_X_RIGHT = (1 << 1),
    /* Distance between window's horizontal center and the screen's horizontal center is constant */
    ANCHOR_X_CENTER = (1 << 2),
    ANCHOR_Y_TOP = (1 << 3),
    ANCHOR_Y_BOT = (1 << 4),
    ANCHOR_Y_CENTER = (1 << 5),
    ANCHOR_DEFAULT = ANCHOR_X_LEFT | ANCHOR_Y_TOP,
    ANCHOR_X_MASK = ANCHOR_X_LEFT | ANCHOR_X_CENTER | ANCHOR_X_RIGHT,
    ANCHOR_Y_MASK = ANCHOR_Y_TOP | ANCHOR_Y_CENTER | ANCHOR_Y_BOT
};

bool UI_Init(const char *basedir, SDL_Window *win);
void UI_Shutdown(void);
void UI_InputBegin(void);
void UI_InputEnd(void);
void UI_HandleEvent(SDL_Event *evt);
void UI_DrawText(const char *text, struct rect rect, struct rgba rgba);
vec2_t UI_GetTextVres(void);

/* Returns a trimmed version of the virtual resolution when the aspect ratio of the window is
 * different than the virtual resolution aspect ratio. The adjusted resolution has the same
 * aspect ratio as the window and has the largest possible dimensions such that it still 'fits'
 * into the original virtual resolution. */
vec2_t UI_ArAdjustedVRes(vec2_t vres);
struct rect UI_BoundsForAspectRatio(struct rect from_bounds, vec2_t from_res,
                                    vec2_t to_res, int resize_mask);
struct nk_context *UI_GetContext(void);
void UI_ClearState(void);

const char *UI_GetActiveFont(void);
bool UI_SetActiveFont(const char *name);

#endif /* _UI_H_ */
