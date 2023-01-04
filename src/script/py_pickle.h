#ifndef _PY_PICKLE_H_
#define _PY_PICKLE_H_

#include <Python.h> /* Must be included first */

#include "../lib/public/khash.h"
#include "../lib/public/vec.h"

#include <stdbool.h>

struct SDL_RWops;

VEC_TYPE(pobj, PyObject *)
VEC_PROTOTYPES(extern, pobj, PyObject *)

bool S_Pickle_Init(PyObject *module);
void S_Pickle_Clear(void);
void S_Pickle_Shutdown(void);

bool S_PickleObjgraph(PyObject *obj, struct SDL_RWops *stream);
/* Returns a new reference */
PyObject *S_UnpickleObjgraph(struct SDL_RWops *stream);
PyObject *S_Pickle_PlainHeapSubtype(PyTypeObject *type);

/* Expose some methods of the pickling module for use types implementing
 * their own pickling and unpickling routines and needing to deal with
 * self-referencing cases.
 */
struct py_pickle_ctx {
    void *private_ctx;
    struct SDL_RWops *stream;
    bool (*memo_contains)(void *ctx, PyObject *obj);
    void (*memoize)(void *ctx, PyObject *obj);
    bool (*emit_put)(void *ctx, PyObject *obj, struct SDL_RWops *stream);
    bool (*emit_get)(void *ctx, PyObject *obj, struct SDL_RWops *stream);
    bool (*emit_alloc)(void *ctx, struct SDL_RWops *stream);
    bool (*pickle_obj)(void *ctx, PyObject *obj, struct SDL_RWops *stream);
    void (*deferred_free)(void *ctx, PyObject *obj);
};

struct py_unpickle_ctx {
    vec_pobj_t *stack;
};

#endif /* _PY_PICKLE_H_ */
