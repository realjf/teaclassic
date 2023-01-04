#ifndef _PY_ERROR_H_
#define _PY_ERROR_H_

#include <Python.h>
#include <stdbool.h>
#include "../game/public/game.h"

struct py_err_ctx {
    bool occurred;
    enum simstate prev_state;
    PyObject *type;
    PyObject *value;
    PyObject *traceback;
};

void S_Error_Update(struct py_err_ctx *err_ctx);

#endif /* _PY_ERROR_H_ */
