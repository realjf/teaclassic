#ifndef _PY_UI_STYLE_H_
#define _PY_UI_STYLE_H_

#include <Python.h> /* must be first */
#include <stdbool.h>

struct nk_context;
struct nk_style_window;
struct SDL_RWops;

bool S_UI_Style_Init(void);
void S_UI_Style_Shutdown(void);
void S_UI_Style_PyRegister(PyObject *module, struct nk_context *ctx);

bool S_UI_Style_SaveWindow(struct SDL_RWops *stream, const struct nk_style_window *window);
bool S_UI_Style_LoadWindow(struct SDL_RWops *stream, struct nk_style_window *out);

PyObject *S_UIHeaderStyleNew(void);
size_t S_UIHeaderGetHeight(PyObject *obj, struct nk_context *ctx);
void S_UIHeaderStylePush(PyObject *obj, struct nk_context *ctx);
void S_UIHeaderStylePop(PyObject *obj, struct nk_context *ctx);


#endif /* _PY_UI_STYLE_H_ */
