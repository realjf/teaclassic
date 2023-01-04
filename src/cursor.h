#ifndef _CURSOR_H_
#define _CURSOR_H_

#include <stdbool.h>

struct SDL_RWops;

enum cursortype {
    CURSOR_POINTER = 0,
    CURSOR_SCROLL_TOP,
    CURSOR_SCROLL_TOP_RIGHT,
    CURSOR_SCROLL_RIGHT,
    CURSOR_SCROLL_BOT_RIGHT,
    CURSOR_SCROLL_BOT,
    CURSOR_SCROLL_BOT_LEFT,
    CURSOR_SCROLL_LEFT,
    CURSOR_SCROLL_TOP_LEFT,
    CURSOR_TARGET,
    CURSOR_ATTACK,
    CURSOR_NO_ATTACK,
    CURSOR_BUILD,
    CURSOR_DROP_OFF,
    CURSOR_TRANSPORT,
    _CURSOR_MAX
};

bool Cursor_InitDefault(const char *basedir);
void Cursor_FreeAll(void);

void Cursor_SetActive(enum cursortype type);
bool Cursor_LoadBMP(enum cursortype type, const char *path, int hotx, int hoty);

/* When RTS mode is set, an event handler will continuosly update the cursor icon to be
 * the correct scrolling icon for the cursor's current position on the screen
 * Must be called after Event subsystem is initialized. */
void Cursor_SetRTSMode(bool on);
bool Cursor_GetRTSMode(void);
void Cursor_SetRTSPointer(enum cursortype type);

bool Cursor_NamedLoadBMP(const char *name, const char *path, int hotx, int hoty);
bool Cursor_NamedSetActive(const char *name);
bool Cursor_NamedSetRTSPointer(const char *name);

void Cursor_ClearState(void);
bool Cursor_SaveState(struct SDL_RWops *stream);
bool Cursor_LoadState(struct SDL_RWops *stream);

#endif /* _CURSOR_H_ */
