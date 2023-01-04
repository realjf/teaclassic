#ifndef _PY_UI_H_
#define _PY_UI_H_

#include <Python.h> /* must be first */
#include <stdbool.h>

struct nk_context;

bool S_UI_Init(struct nk_context *ctx);
void S_UI_Shutdown(void);
void S_UI_Update(void);
void S_UI_PyRegister(PyObject *module);
PyObject *S_UI_ActiveWindow(void);


#endif /* _PY_UI_H_ */
