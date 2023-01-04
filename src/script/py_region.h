#ifndef _PY_REGION_H_
#define _PY_REGION_H_

#include <Python.h> /* must be first */
#include <stdbool.h>

bool S_Region_Init(void);
void S_Region_Shutdown(void);
void S_Region_Clear(void);
void S_Region_PyRegister(PyObject *module);
PyObject *S_Region_GetLoaded(void);

#endif /* _PY_REGION_H_ */
