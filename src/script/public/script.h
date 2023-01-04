#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include "../../scene.h"
#include "../../tc_math.h"

#include <stdio.h>
#include <stdbool.h>
#include <SDL.h> /* for SDL_RWops */

/* 'Handle' type to let the rest of the engine hold on to scripting objects
 * without needing to include Python.h */
typedef void *script_opaque_t;

enum eventtype;
struct nk_context;

/*###########################################################################*/
/* SCRIPT GENERAL                                                            */
/*###########################################################################*/

bool S_Init(const char *progname, const char *base_path, struct nk_context *ctx);
void S_Shutdown(void);
bool S_RunFile(const char *path, int argc, char **argv);
bool S_GetFilePath(char *out, size_t maxout);
void S_ShowLastError(void);

void S_RunEventHandler(script_opaque_t callable, script_opaque_t user_arg,
                       void *event_arg);

void S_Retain(script_opaque_t obj);
/* Decrement reference count for Python objects.
 * No-op in the case of a NULL-pointer passed in */
void S_Release(script_opaque_t obj);
script_opaque_t S_WrapEngineEventArg(int eventnum, void *arg);
/* Returns 'arg' if this is not a weakref object. Otherwise, return a borrowed
 * reference extracted from the weakref. */
script_opaque_t S_UnwrapIfWeakref(script_opaque_t arg);
bool S_WeakrefDied(script_opaque_t arg);
bool S_ObjectsEqual(script_opaque_t a, script_opaque_t b);
/* This value is not persistent accross sessions - careful */
uint64_t S_ScriptTypeID(uint32_t uid);

void S_ClearState(void);
bool S_SaveState(SDL_RWops *stream);
bool S_LoadState(SDL_RWops *stream);

void S_Task_MaybeExit(void);
void S_Task_MaybeEnter(void);

/*###########################################################################*/
/* SCRIPT UI                                                                 */
/*###########################################################################*/

bool S_UI_MouseOverWindow(int mouse_x, int mouse_y);
bool S_UI_TextEditHasFocus(void);

/*###########################################################################*/
/* SCRIPT ENTITY                                                             */
/*###########################################################################*/

script_opaque_t S_Entity_ObjFromAtts(const char *path, const char *name,
                                     const khash_t(attr) * attr_table,
                                     const vec_attr_t *construct_args);
bool S_Entity_UIDForObj(script_opaque_t, uint32_t *out);
script_opaque_t S_Entity_ObjForUID(uint32_t uid);

/*###########################################################################*/
/* SCRIPT REGION                                                             */
/*###########################################################################*/

void S_Region_NotifyContentsChanged(const char *name);
script_opaque_t S_Region_ObjFromAtts(const char *name, int type, vec2_t pos,
                                     float radius, float xlen, float zlen);

#endif /* _SCRIPT_H_ */
